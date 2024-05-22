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

// 생성자
AEliteZombie::AEliteZombie()
{
    // 틱 활성화
    PrimaryActorTick.bCanEverTick = true;

    // 네트워크 복제 설정
    SetReplicates(true);
    SetReplicateMovement(true);

    // 기본 변수 초기화
    PatrolRadius = 1000.0f;
    TimeToSleep = 10.0f;
    Health = 1000000.0f; // 높은 체력 설정
    AttackRange = 80.0f;
    Damage = 50.f;
    AttackCooldown = 3.f;
    LegDamageAccumulated = 0.f; // 다리 누적 데미지 초기화
}

// 게임 시작 시 초기화
void AEliteZombie::BeginPlay()
{
    Super::BeginPlay();

    // 캐릭터의 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// 데미지를 받을 때 호출되는 함수
float AEliteZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

    // 'Hit reaction' 상태로 전환
    if (ZombieState != EZombieState::HitReact && ZombieInjuryState != EZombieInjuryState::Injured)
    {
        // 움직임 모드를 None으로 설정
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        ZombieState = EZombieState::HitReact;

        // 기존 타이머가 있으면 취소
        GetWorldTimerManager().ClearTimer(TimerHandle_HitReactEnd);

        // 'hit reaction' 상태를 일정 시간 후에 종료하도록 타이머 설정
        GetWorldTimerManager().SetTimer(TimerHandle_HitReactEnd, this, &AEliteZombie::HitReactEnd, 1.0f, false);
    }

    return ActualDamage;
}

// 좀비를 쓰러뜨리는 함수
void AEliteZombie::GetDown()
{
    // 움직임 모드를 None으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // 좀비 부상 상태를 Down으로 변경
    ZombieInjuryState = EZombieInjuryState::Down;

    // 이동 속도를 감소시킴
    GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

// 좀비가 쓰러짐 상태를 끝내는 함수
void AEliteZombie::DownEnd()
{
    // 움직임 모드를 Walking으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 좀비 부상 상태를 Injured로 변경
    ZombieInjuryState = EZombieInjuryState::Injured;

    FTimerHandle TimerHandle_StandUp;

    // 60초 후 StandUp 메서드 호출 설정
    GetWorldTimerManager().SetTimer(TimerHandle_StandUp, this, &AEliteZombie::StandUp, 60.0f, false);
}

// 좀비가 일어나는 함수
void AEliteZombie::StandUp()
{
    // 움직임 모드를 None으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    // 좀비 부상 상태를 Stand로 변경
    ZombieInjuryState = EZombieInjuryState::Stand;
}

// 좀비가 일어나는 상태를 끝내는 함수
void AEliteZombie::StandUpEnd()
{
    // 움직임 모드를 Walking으로 설정
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 좀비 부상 상태와 일반 상태를 초기화
    ZombieInjuryState = EZombieInjuryState::None;
    ZombieState = EZombieState::None;

    // 누적된 다리 데미지 초기화
    LegDamageAccumulated = 0.f;

    // 원래 이동 속도로 복구
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

// 타겟을 공격하는 함수
void AEliteZombie::Attack(APawn* Target)
{
    // 부모 클래스의 Attack 함수 호출
    Super::Attack(Target);
}

// 타겟에게 데미지를 적용하는 함수
void AEliteZombie::ApplyDamageToTarget()
{
    // 부모 클래스의 ApplyDamageToTarget 함수 호출
    Super::ApplyDamageToTarget();
}

// 총격을 받았을 때의 처리 함수
void AEliteZombie::TakeShot(FHitResultData HitResult, float WeaponDamage)
{
    // 부모 클래스의 TakeShot 함수 호출
    Super::TakeShot(HitResult, WeaponDamage);

    // 다리에 적중한 경우 누적 데미지 증가
    if (HitResult.bHitLeg)
    {
        LegDamageAccumulated += WeaponDamage;
    }

    // 누적 데미지가 일정량 이상이면 쓰러짐 상태로 전환
    if (ZombieInjuryState == EZombieInjuryState::None && LegDamageAccumulated >= 100)
    {
        GetDown();
    }
}

// 네트워크 복제 변수 설정
void AEliteZombie::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // 부모 클래스의 GetLifetimeReplicatedProps 함수 호출
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제할 변수들 추가
    DOREPLIFETIME(AEliteZombie, ZombieInjuryState);
    DOREPLIFETIME(AEliteZombie, LegDamageAccumulated);
}
