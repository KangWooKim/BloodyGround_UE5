// Fill out your copyright notice in the Description page of Project Settings.

#include "EliteZombie.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// ������
AEliteZombie::AEliteZombie()
{
    // ƽ Ȱ��ȭ
    PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ��ũ ���� ����
    SetReplicates(true);
    SetReplicateMovement(true);

    // �⺻ ���� �ʱ�ȭ
    PatrolRadius = 1000.0f;
    TimeToSleep = 10.0f;
    Health = 1000000.0f; // ���� ü�� ����
    AttackRange = 80.0f;
    Damage = 50.f;
    AttackCooldown = 3.f;
    LegDamageAccumulated = 0.f; // �ٸ� ���� ������ �ʱ�ȭ
}

// ���� ���� �� �ʱ�ȭ
void AEliteZombie::BeginPlay()
{
    Super::BeginPlay();

    // ĳ������ �̵� �ӵ� ����
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// �������� ���� �� ȣ��Ǵ� �Լ�
float AEliteZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

    // 'Hit reaction' ���·� ��ȯ
    if (ZombieState != EZombieState::HitReact && ZombieInjuryState != EZombieInjuryState::Injured)
    {
        // ������ ��带 None���� ����
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        ZombieState = EZombieState::HitReact;

        // ���� Ÿ�̸Ӱ� ������ ���
        GetWorldTimerManager().ClearTimer(TimerHandle_HitReactEnd);

        // 'hit reaction' ���¸� ���� �ð� �Ŀ� �����ϵ��� Ÿ�̸� ����
        GetWorldTimerManager().SetTimer(TimerHandle_HitReactEnd, this, &AEliteZombie::HitReactEnd, 1.0f, false);
    }

    return ActualDamage;
}

// ���� �����߸��� �Լ�
void AEliteZombie::GetDown()
{
    // ������ ��带 None���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // ���� �λ� ���¸� Down���� ����
    ZombieInjuryState = EZombieInjuryState::Down;

    // �̵� �ӵ��� ���ҽ�Ŵ
    GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

// ���� ������ ���¸� ������ �Լ�
void AEliteZombie::DownEnd()
{
    // ������ ��带 Walking���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // ���� �λ� ���¸� Injured�� ����
    ZombieInjuryState = EZombieInjuryState::Injured;

    FTimerHandle TimerHandle_StandUp;

    // 60�� �� StandUp �޼��� ȣ�� ����
    GetWorldTimerManager().SetTimer(TimerHandle_StandUp, this, &AEliteZombie::StandUp, 60.0f, false);
}

// ���� �Ͼ�� �Լ�
void AEliteZombie::StandUp()
{
    // ������ ��带 None���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // ���� �λ� ���¸� Stand�� ����
    ZombieInjuryState = EZombieInjuryState::Stand;
}

// ���� �Ͼ�� ���¸� ������ �Լ�
void AEliteZombie::StandUpEnd()
{
    // ������ ��带 Walking���� ����
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // ���� �λ� ���¿� �Ϲ� ���¸� �ʱ�ȭ
    ZombieInjuryState = EZombieInjuryState::None;
    ZombieState = EZombieState::None;

    // ������ �ٸ� ������ �ʱ�ȭ
    LegDamageAccumulated = 0.f;

    // ���� �̵� �ӵ��� ����
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// Ÿ���� �����ϴ� �Լ�
void AEliteZombie::Attack(APawn* Target)
{
    // �θ� Ŭ������ Attack �Լ� ȣ��
    Super::Attack(Target);
}

// Ÿ�ٿ��� �������� �����ϴ� �Լ�
void AEliteZombie::ApplyDamageToTarget()
{
    // �θ� Ŭ������ ApplyDamageToTarget �Լ� ȣ��
    Super::ApplyDamageToTarget();
}

// �Ѱ��� �޾��� ���� ó�� �Լ�
void AEliteZombie::TakeShot(FHitResultData HitResult, float WeaponDamage)
{
    // �θ� Ŭ������ TakeShot �Լ� ȣ��
    Super::TakeShot(HitResult, WeaponDamage);

    // �ٸ��� ������ ��� ���� ������ ����
    if (HitResult.bHitLeg)
    {
        LegDamageAccumulated += WeaponDamage;
    }

    // ���� �������� ������ �̻��̸� ������ ���·� ��ȯ
    if (ZombieInjuryState == EZombieInjuryState::None && LegDamageAccumulated >= 100)
    {
        GetDown();
    }
}

// ��Ʈ��ũ ���� ���� ����
void AEliteZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // �θ� Ŭ������ GetLifetimeReplicatedProps �Լ� ȣ��
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // ������ ������ �߰�
    DOREPLIFETIME(AEliteZombie, ZombieInjuryState);
    DOREPLIFETIME(AEliteZombie, LegDamageAccumulated);
}
