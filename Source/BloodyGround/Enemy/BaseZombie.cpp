#include "BaseZombie.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Component/ServerLocationComponent.h"
#include "BloodyGround/GameState/BloodyGroundGameState.h"

// ������
ABaseZombie::ABaseZombie()
{
    // ƽ Ȱ��ȭ
    PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ��ũ ���� ����
    SetReplicates(true);
    SetReplicateMovement(true);

    // PawnSensingComponent ���� �� ����
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 5000.f;
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);
    PawnSensingComp->HearingThreshold = 3000.f;
    PawnSensingComp->LOSHearingThreshold = 2800.f;
    PawnSensingComp->bOnlySensePlayers = false;

    // ���� ���� �� ��Ÿ ���� �ʱ�ȭ
    ZombieState = EZombieState::None;
    PatrolRadius = 1000.0f;
    TimeToSleep = 5.0f;
    Health = 50.0f;
    AttackRange = 80.0f;
    Damage = 20.f;
    LastAttackTime = -AttackCooldown;

    // ĳ���� ������ ����
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ���� ��ġ ������Ʈ ���� �� ����
    ServerLocationComp = CreateDefaultSubobject<UServerLocationComponent>(TEXT("ServerLocationComp"));
    if (ServerLocationComp)
    {
        ServerLocationComp->GetBodyComponent()->SetupAttachment(GetMesh(), FName("Spine"));
        ServerLocationComp->GetLeftLegComponent()->SetupAttachment(GetMesh(), FName("LeftLeg"));
        ServerLocationComp->GetRightLegComponent()->SetupAttachment(GetMesh(), FName("RightLeg"));
    }
}

void ABaseZombie::BeginPlay()
{
    Super::BeginPlay();

    // �ʱ� ���� ���� ����
    ZombieState = EZombieState::None;

    // ������ �̵� �ӵ� ����
    GetCharacterMovement()->MaxWalkSpeed = 150.0f;
    PatrolPoint = GetRandomPatrolPoint();

    // ���� �±� �߰�
    Tags.AddUnique(FName("Zombie"));
}

// ��Ʈ��ũ ���� ���� ����
void ABaseZombie::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // ������ ������ �߰�
    DOREPLIFETIME(ABaseZombie, ZombieState);
    DOREPLIFETIME(ABaseZombie, CurrentTarget);
    DOREPLIFETIME(ABaseZombie, PatrolPoint);
    DOREPLIFETIME(ABaseZombie, Health);
}

void ABaseZombie::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // PawnSensingComponent �̺�Ʈ �ڵ鷯 �߰�
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);
}

void ABaseZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���� ���� Ȯ��
    ABloodyGroundGameState* GameState = GetWorld()->GetGameState<ABloodyGroundGameState>();
    if (!GameState) return;

    // �� ������ ���� ó��
    if (GameState->DayNightCycle == EDayNightCycle::Night)
    {
        if (ZombieState != EZombieState::Sleep && !CurrentTarget)
        {
            if (!GetWorldTimerManager().IsTimerActive(SleepTimerHandle))
            {
                GetWorldTimerManager().SetTimer(SleepTimerHandle, this, &ABaseZombie::GoBackToSleep, TimeToSleep, false);
            }
        }
    }
    else
    {
        if (GetWorldTimerManager().IsTimerActive(SleepTimerHandle))
        {
            GetWorldTimerManager().ClearTimer(SleepTimerHandle);
        }
    }

    // ���� Ÿ��(�÷��̾�)�� ����ߴ��� Ȯ��
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(CurrentTarget);
    if (PlayerCharacter && PlayerCharacter->GetHealth() <= 0)
    {
        CurrentTarget = nullptr;
        RestartPatrol();
    }

    // ������ ���� ���¿� ���� ó��
    if (ZombieState == EZombieState::Attacking || ZombieState == EZombieState::Sleep || ZombieState == EZombieState::HitReact)
    {
        return;
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        if (CurrentTarget)
        {
            // Ÿ���� ���� ���� ���� �ִ� ��� ����
            if (FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
            {
                float CurrentTime = GetWorld()->GetTimeSeconds();
                if (CurrentTime - LastAttackTime >= AttackCooldown)
                {
                    Attack(CurrentTarget);
                    LastAttackTime = CurrentTime;
                }
            }
            else if (!IsTargetInSight(CurrentTarget))
            {
                // Ÿ���� �þ� ���� ������ ���� ���
                CurrentTarget = nullptr;
                RestartPatrol();
            }
            else
            {
                // Ÿ���� ���� ������ ������� �þ� ���� �ִ� ���
                ZombieState = EZombieState::None;
                AIController->MoveToActor(CurrentTarget);
            }
        }
        else
        {
            // Ÿ���� ���� ��� ������ ��� ����
            if (AIController->GetPathFollowingComponent()->DidMoveReachGoal())
            {
                PatrolPoint = GetRandomPatrolPoint();
                AIController->MoveToLocation(PatrolPoint);
            }
        }
    }
}

// ���� ���� ����Ʈ�� ��� �Լ�
FVector ABaseZombie::GetRandomPatrolPoint()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation ResultLocation;
    if (NavSys && NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), PatrolRadius, ResultLocation))
    {
        return ResultLocation.Location;
    }
    return GetActorLocation();
}

// �÷��̾ �þ� ���� �߰����� �� ȣ��Ǵ� �Լ�
void ABaseZombie::OnSeePlayer(APawn* Pawn)
{
    // ����� �÷��̾�� ����
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(Pawn);
    if (ZombieState == EZombieState::Sleep || !PlayerCharacter || PlayerCharacter->GetHealth() <= 0)
    {
        return;
    }

    // ���� ���� �ִ� ��� ����
    if (ZombieState == EZombieState::Sleep)
    {
        return;
    }

    if (CurrentTarget) return;

    if (ZombieState == EZombieState::None && Pawn && Pawn->ActorHasTag(FName("Player")))
    {
        AAIController* AIController = Cast<AAIController>(GetController());
        if (AIController)
        {
            AIController->MoveToActor(Pawn);
            CurrentTarget = Pawn;
            LastSeenTime = GetWorld()->GetTimeSeconds();
        }
    }
}

// ������ ����� �� ȣ��Ǵ� �Լ�
void ABaseZombie::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
    // ����� �÷��̾ ���� ���� ����
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(NoiseInstigator);
    if (ZombieState == EZombieState::Sleep || (PlayerCharacter && PlayerCharacter->GetHealth() <= 0))
    {
        WakeUp();
        return;
    }

    // ���� ���� �ִ� ��� ����
    if (ZombieState == EZombieState::Sleep)
    {
        WakeUp();
    }

    // ������ �� ����� �����̰ų� �̹� Ÿ���� �ִ� ��� ����
    if (NoiseInstigator->ActorHasTag(FName("Zombie")) || CurrentTarget != nullptr)
    {
        return;
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->MoveToLocation(Location);
        CurrentTarget = NoiseInstigator;
    }
}

// ���� ����� �Լ�
void ABaseZombie::WakeUp()
{
    // ���¸� None���� ����
    ZombieState = EZombieState::None;

    // Ÿ�̸Ӱ� �����Ǿ� �ִٸ� ���
    if (GetWorldTimerManager().IsTimerActive(SleepTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(SleepTimerHandle);
    }
}

// ���� �ٽ� ������ �Լ�
void ABaseZombie::GoBackToSleep()
{
    // ���¸� Sleep���� ����
    ZombieState = EZombieState::Sleep;

    // ���� �� ��Ÿ Ȱ�� �ߴ�
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->StopMovement();
    }
}

// Ÿ���� �����ϴ� �Լ�
void ABaseZombie::Attack(APawn* Target)
{
    // ���� ���°� None�̰� Ÿ���� BaseCharacter�� ��� ����
    if (ZombieState == EZombieState::None && Target->IsA(ABaseCharacter::StaticClass()))
    {
        ZombieState = EZombieState::Attacking;
    }
}

// Ÿ�ٿ��� �������� �����ϴ� �Լ�
void ABaseZombie::ApplyDamageToTarget()
{
    // �������� ����
    if (HasAuthority())
    {
        // Ÿ���� ���� ���� ���� ���� ��� ������ ����
        if (CurrentTarget && FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
        {
            UGameplayStatics::ApplyDamage(CurrentTarget, Damage, GetController(), this, UDamageType::StaticClass());
        }

        // ���¸� None���� ����
        ZombieState = EZombieState::None;
    }
}

// ���� �������� ���� �� ȣ��Ǵ� �Լ�
float ABaseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // ���� ���񿡰Լ� �޴� �������� ����
    if (DamageCauser && DamageCauser->IsA(ABaseZombie::StaticClass()))
    {
        return 0.0f;
    }

    // �̹� ����� ��� ������ ����
    if (ZombieState == EZombieState::Death)
    {
        return 0.0f;
    }

    // ���� ������ ����
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health -= ActualDamage;

    // ���� ����� ���
    if (Health <= 0)
    {
        HandleDeath();
        return ActualDamage;
    }

    // ���� ���� �ִ� ��� ����
    if (ZombieState == EZombieState::Sleep)
    {
        WakeUp();
        CurrentTarget = Cast<APawn>(DamageCauser);
    }

    // 'Hit reaction' ���·� ��ȯ
    if (ZombieState != EZombieState::HitReact)
    {
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        ZombieState = EZombieState::HitReact;

        // ���� Ÿ�̸Ӱ� ������ ���
        GetWorldTimerManager().ClearTimer(TimerHandle_HitReactEnd);

        // ��Ʈ ���׼� ���� Ÿ�̸� ����
        GetWorldTimerManager().SetTimer(TimerHandle_HitReactEnd, this, &ABaseZombie::HitReactEnd, 1.0f, false);

        CurrentTarget = Cast<APawn>(DamageCauser);
    }

    return ActualDamage;
}

// ��Ʈ ���׼� ���� �Լ�
void ABaseZombie::HitReactEnd()
{
    // ������ ��带 Walking���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // ���¸� None���� ����
    ZombieState = EZombieState::None;
}

// ���� ��� ó�� �Լ�
void ABaseZombie::HandleDeath()
{
    // ������ ��带 None���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_None);
    // ���¸� Death�� ����
    ZombieState = EZombieState::Death;
    // ���� Ÿ���� ����
    CurrentTarget = nullptr;
}

// ��� �ִϸ��̼� ���� �Լ�
void ABaseZombie::DeathEnd()
{
    // ���� ����
    Super::Destroy();
    Destroy();
}

// ������ ������ϴ� �Լ�
void ABaseZombie::RestartPatrol()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        PatrolPoint = GetRandomPatrolPoint();
        AIController->MoveToLocation(PatrolPoint);
    }
}

// Ÿ���� �þ� ���� �ִ��� Ȯ���ϴ� �Լ�
bool ABaseZombie::IsTargetInSight(APawn* Target)
{
    if (!Target)
    {
        return false;
    }

    // ���� �ð��� ���������� �÷��̾ ������ �ð��� ���� ���
    float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

    // ���� ���, 5�� �̳��� Ÿ���� �����ߴٸ� �þ� ���� ���� ����
    return TimeSinceLastSeen < 5.0f;
}

// �Ѱ��� �޾��� ���� ó�� �Լ� (�������� ����)
void ABaseZombie::TakeShot(FHitResultData HitResult, float WeaponDamage)
{
}
