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

// 생성자
ABaseZombie::ABaseZombie()
{
    // 틱 활성화
    PrimaryActorTick.bCanEverTick = true;

    // 네트워크 복제 설정
    SetReplicates(true);
    SetReplicateMovement(true);

    // PawnSensingComponent 생성 및 설정
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 5000.f;
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);
    PawnSensingComp->HearingThreshold = 3000.f;
    PawnSensingComp->LOSHearingThreshold = 2800.f;
    PawnSensingComp->bOnlySensePlayers = false;

    // 좀비 상태 및 기타 변수 초기화
    ZombieState = EZombieState::None;
    PatrolRadius = 1000.0f;
    TimeToSleep = 5.0f;
    Health = 50.0f;
    AttackRange = 80.0f;
    Damage = 20.f;
    LastAttackTime = -AttackCooldown;

    // 캐릭터 움직임 설정
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // 서버 위치 컴포넌트 생성 및 부착
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

    // 초기 좀비 상태 설정
    ZombieState = EZombieState::None;

    // 좀비의 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = 150.0f;
    PatrolPoint = GetRandomPatrolPoint();

    // 좀비 태그 추가
    Tags.AddUnique(FName("Zombie"));
}

// 네트워크 복제 변수 설정
void ABaseZombie::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제할 변수들 추가
    DOREPLIFETIME(ABaseZombie, ZombieState);
    DOREPLIFETIME(ABaseZombie, CurrentTarget);
    DOREPLIFETIME(ABaseZombie, PatrolPoint);
    DOREPLIFETIME(ABaseZombie, Health);
}

void ABaseZombie::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // PawnSensingComponent 이벤트 핸들러 추가
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);
}

void ABaseZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 게임 상태 확인
    ABloodyGroundGameState* GameState = GetWorld()->GetGameState<ABloodyGroundGameState>();
    if (!GameState) return;

    // 밤 상태일 때의 처리
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

    // 현재 타겟(플레이어)이 사망했는지 확인
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(CurrentTarget);
    if (PlayerCharacter && PlayerCharacter->GetHealth() <= 0)
    {
        CurrentTarget = nullptr;
        RestartPatrol();
    }

    // 좀비의 현재 상태에 따른 처리
    if (ZombieState == EZombieState::Attacking || ZombieState == EZombieState::Sleep || ZombieState == EZombieState::HitReact)
    {
        return;
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        if (CurrentTarget)
        {
            // 타겟이 공격 범위 내에 있는 경우 공격
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
                // 타겟이 시야 범위 밖으로 나간 경우
                CurrentTarget = nullptr;
                RestartPatrol();
            }
            else
            {
                // 타겟이 공격 범위를 벗어났지만 시야 내에 있는 경우
                ZombieState = EZombieState::None;
                AIController->MoveToActor(CurrentTarget);
            }
        }
        else
        {
            // 타겟이 없는 경우 순찰을 계속 수행
            if (AIController->GetPathFollowingComponent()->DidMoveReachGoal())
            {
                PatrolPoint = GetRandomPatrolPoint();
                AIController->MoveToLocation(PatrolPoint);
            }
        }
    }
}

// 랜덤 순찰 포인트를 얻는 함수
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

// 플레이어를 시야 내에 발견했을 때 호출되는 함수
void ABaseZombie::OnSeePlayer(APawn* Pawn)
{
    // 사망한 플레이어는 무시
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(Pawn);
    if (ZombieState == EZombieState::Sleep || !PlayerCharacter || PlayerCharacter->GetHealth() <= 0)
    {
        return;
    }

    // 좀비가 잠들어 있는 경우 무시
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

// 소음을 들었을 때 호출되는 함수
void ABaseZombie::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
    // 사망한 플레이어에 대한 반응 무시
    ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(NoiseInstigator);
    if (ZombieState == EZombieState::Sleep || (PlayerCharacter && PlayerCharacter->GetHealth() <= 0))
    {
        WakeUp();
        return;
    }

    // 좀비가 잠들어 있는 경우 깨움
    if (ZombieState == EZombieState::Sleep)
    {
        WakeUp();
    }

    // 소음을 낸 대상이 좀비이거나 이미 타겟이 있는 경우 무시
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

// 좀비를 깨우는 함수
void ABaseZombie::WakeUp()
{
    // 상태를 None으로 변경
    ZombieState = EZombieState::None;

    // 타이머가 설정되어 있다면 취소
    if (GetWorldTimerManager().IsTimerActive(SleepTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(SleepTimerHandle);
    }
}

// 좀비를 다시 잠재우는 함수
void ABaseZombie::GoBackToSleep()
{
    // 상태를 Sleep으로 변경
    ZombieState = EZombieState::Sleep;

    // 순찰 및 기타 활동 중단
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->StopMovement();
    }
}

// 타겟을 공격하는 함수
void ABaseZombie::Attack(APawn* Target)
{
    // 좀비 상태가 None이고 타겟이 BaseCharacter일 경우 공격
    if (ZombieState == EZombieState::None && Target->IsA(ABaseCharacter::StaticClass()))
    {
        ZombieState = EZombieState::Attacking;
    }
}

// 타겟에게 데미지를 적용하는 함수
void ABaseZombie::ApplyDamageToTarget()
{
    // 서버에서 실행
    if (HasAuthority())
    {
        // 타겟이 공격 범위 내에 있을 경우 데미지 적용
        if (CurrentTarget && FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
        {
            UGameplayStatics::ApplyDamage(CurrentTarget, Damage, GetController(), this, UDamageType::StaticClass());
        }

        // 상태를 None으로 변경
        ZombieState = EZombieState::None;
    }
}

// 좀비가 데미지를 받을 때 호출되는 함수
float ABaseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 좀비가 좀비에게서 받는 데미지는 무시
    if (DamageCauser && DamageCauser->IsA(ABaseZombie::StaticClass()))
    {
        return 0.0f;
    }

    // 이미 사망한 경우 데미지 무시
    if (ZombieState == EZombieState::Death)
    {
        return 0.0f;
    }

    // 실제 데미지 적용
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health -= ActualDamage;

    // 좀비가 사망한 경우
    if (Health <= 0)
    {
        HandleDeath();
        return ActualDamage;
    }

    // 좀비가 잠들어 있는 경우 깨움
    if (ZombieState == EZombieState::Sleep)
    {
        WakeUp();
        CurrentTarget = Cast<APawn>(DamageCauser);
    }

    // 'Hit reaction' 상태로 전환
    if (ZombieState != EZombieState::HitReact)
    {
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        ZombieState = EZombieState::HitReact;

        // 기존 타이머가 있으면 취소
        GetWorldTimerManager().ClearTimer(TimerHandle_HitReactEnd);

        // 히트 리액션 종료 타이머 설정
        GetWorldTimerManager().SetTimer(TimerHandle_HitReactEnd, this, &ABaseZombie::HitReactEnd, 1.0f, false);

        CurrentTarget = Cast<APawn>(DamageCauser);
    }

    return ActualDamage;
}

// 히트 리액션 종료 함수
void ABaseZombie::HitReactEnd()
{
    // 움직임 모드를 Walking으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 상태를 None으로 변경
    ZombieState = EZombieState::None;
}

// 좀비 사망 처리 함수
void ABaseZombie::HandleDeath()
{
    // 움직임 모드를 None으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_None);
    // 상태를 Death로 변경
    ZombieState = EZombieState::Death;
    // 현재 타겟을 제거
    CurrentTarget = nullptr;
}

// 사망 애니메이션 종료 함수
void ABaseZombie::DeathEnd()
{
    // 좀비 제거
    Super::Destroy();
    Destroy();
}

// 순찰을 재시작하는 함수
void ABaseZombie::RestartPatrol()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        PatrolPoint = GetRandomPatrolPoint();
        AIController->MoveToLocation(PatrolPoint);
    }
}

// 타겟이 시야 내에 있는지 확인하는 함수
bool ABaseZombie::IsTargetInSight(APawn* Target)
{
    if (!Target)
    {
        return false;
    }

    // 현재 시간과 마지막으로 플레이어를 감지한 시간의 차이 계산
    float TimeSinceLastSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;

    // 예를 들어, 5초 이내에 타겟을 감지했다면 시야 범위 내로 간주
    return TimeSinceLastSeen < 5.0f;
}

// 총격을 받았을 때의 처리 함수 (구현되지 않음)
void ABaseZombie::TakeShot(FHitResultData HitResult, float WeaponDamage)
{
}
