// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "EliteZombie.generated.h"

// 좀비 부상 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class EZombieInjuryState : uint8
{
    None    UMETA(DisplayName = "None"),       // 부상 없음
    Injured  UMETA(DisplayName = "Injured"),   // 부상 상태
    Down  UMETA(DisplayName = "Down"),         // 쓰러진 상태
    Stand  UMETA(DisplayName = "Stand")        // 일어나는 상태
};

/**
 * 엘리트 좀비 클래스
 */
UCLASS()
class BLOODYGROUND_API AEliteZombie : public ABaseZombie
{
    GENERATED_BODY()

public:

    // 생성자
    AEliteZombie();

    // 현재 좀비 부상 상태를 반환
    FORCEINLINE EZombieInjuryState GetZombieInjuryState() { return ZombieInjuryState; }

    // 오버라이드된 데미지 처리 함수
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // 좀비를 쓰러뜨리는 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void GetDown();

    // 좀비가 쓰러짐 상태를 끝내는 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void DownEnd();

    // 좀비가 일어나는 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void StandUp();

    // 좀비가 일어나는 상태를 끝내는 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void StandUpEnd();

    // 타겟을 공격하는 함수 (오버라이드)
    virtual void Attack(APawn* Target) override;

    // 타겟에게 데미지를 적용하는 함수 (오버라이드)
    virtual void ApplyDamageToTarget() override;

    // 총격을 받았을 때의 처리 함수 (오버라이드)
    virtual void TakeShot(FHitResultData HitResult, float WeaponDamage) override;

protected:

    // 컴포넌트가 게임 시작 시 초기화되는 함수
    virtual void BeginPlay() override;

    // 네트워크 복제를 위한 속성 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 다리에 대한 데미지 누적
    UPROPERTY(Replicated)
        float LegDamageAccumulated;

private:

    // 좀비 부상 상태
    UPROPERTY(Replicated)
        EZombieInjuryState ZombieInjuryState;
};
