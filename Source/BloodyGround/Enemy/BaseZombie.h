#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "BloodyGround/Component/ServerLocationComponent.h"
#include "BaseZombie.generated.h"

// 좀비 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class EZombieState : uint8
{
    None    UMETA(DisplayName = "None"),          // 아무 상태 아님
    Attacking  UMETA(DisplayName = "Attacking"),  // 공격 중
    HitReact  UMETA(DisplayName = "HitReact"),    // 피격 반응 중
    Death  UMETA(DisplayName = "Death"),          // 사망 상태
    Sleep  UMETA(DisplayName = "Sleep"),          // 잠든 상태
    MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class BLOODYGROUND_API ABaseZombie : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseZombie();

    // 총격을 받았을 때의 처리 함수
    UFUNCTION()
        virtual void TakeShot(FHitResultData HitResult, float WeaponDamage);

    // 현재 좀비 상태를 반환
    FORCEINLINE EZombieState GetZombieState() { return ZombieState; }

    // 서버 위치 컴포넌트를 반환
    FORCEINLINE UServerLocationComponent* GetServerLocationComponent() { return ServerLocationComp; }

protected:
    // 컴포넌트가 게임 시작 시 초기화되는 함수
    virtual void BeginPlay() override;

    // 네트워크 복제를 위한 속성 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 컴포넌트가 초기화될 때 호출되는 함수
    virtual void PostInitializeComponents() override;

    // 피격 및 사망 처리 관련 함수
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // 사망 처리 함수
    void HandleDeath();

    // 매 프레임마다 호출되는 틱 함수
    virtual void Tick(float DeltaTime) override;

    // 플레이어를 시야 내에 발견했을 때 호출되는 함수
    UFUNCTION()
        void OnSeePlayer(APawn* Pawn);

    // 타겟이 시야 내에 있는지 확인하는 함수
    UFUNCTION()
        bool IsTargetInSight(APawn* Target);

    // 순찰을 재시작하는 함수
    UFUNCTION()
        void RestartPatrol();

    // 소음을 들었을 때 호출되는 함수
    UFUNCTION()
        void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

    // 타겟을 공격하는 함수
    UFUNCTION()
        virtual void Attack(APawn* Target);

    // 타겟에게 데미지를 적용하는 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        virtual void ApplyDamageToTarget();

    // 히트 리액션 종료 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void HitReactEnd();

    // 사망 애니메이션 종료 함수
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void DeathEnd();

    // 공격 대미지
    UPROPERTY(EditAnywhere)
        float Damage;

    // 좀비의 체력
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Health")
        float Health;

    // 공격 가능 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Combat")
        float AttackRange;

    // 순찰 반경
    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float PatrolRadius;

    // 잠들기까지의 시간
    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float TimeToSleep;

    // 현재 추적 중인 타겟
    UPROPERTY(Replicated)
        APawn* CurrentTarget;

    // 좀비 상태
    UPROPERTY(Replicated)
        EZombieState ZombieState;

    // 공격 대기 시간 (초)
    float AttackCooldown = 5.0f;

    // 히트 리액션 종료 타이머 핸들
    FTimerHandle TimerHandle_HitReactEnd;

private:
    // 랜덤 순찰 포인트를 얻는 함수
    FVector GetRandomPatrolPoint();

    // 좀비를 깨우는 함수
    void WakeUp();

    // 좀비를 다시 잠재우는 함수
    void GoBackToSleep();

    // 마지막 공격 시간
    UPROPERTY()
        float LastAttackTime;

    // PawnSensingComponent
    UPROPERTY(VisibleAnywhere, Category = "Zombie|Components")
        UPawnSensingComponent* PawnSensingComp;

    // 순찰 포인트
    UPROPERTY(Replicated)
        FVector PatrolPoint;

    // 마지막으로 플레이어를 감지한 시간
    UPROPERTY()
        float LastSeenTime;

    // 서버 위치 컴포넌트
    UPROPERTY()
        class UServerLocationComponent* ServerLocationComp;

    // 잠자기 타이머 핸들
    UPROPERTY()
        FTimerHandle SleepTimerHandle;
};
