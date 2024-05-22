#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "BloodyGround/Component/ServerLocationComponent.h"
#include "BaseZombie.generated.h"

// ���� ���¸� ��Ÿ���� ������
UENUM(BlueprintType)
enum class EZombieState : uint8
{
    None    UMETA(DisplayName = "None"),          // �ƹ� ���� �ƴ�
    Attacking  UMETA(DisplayName = "Attacking"),  // ���� ��
    HitReact  UMETA(DisplayName = "HitReact"),    // �ǰ� ���� ��
    Death  UMETA(DisplayName = "Death"),          // ��� ����
    Sleep  UMETA(DisplayName = "Sleep"),          // ��� ����
    MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class BLOODYGROUND_API ABaseZombie : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseZombie();

    // �Ѱ��� �޾��� ���� ó�� �Լ�
    UFUNCTION()
        virtual void TakeShot(FHitResultData HitResult, float WeaponDamage);

    // ���� ���� ���¸� ��ȯ
    FORCEINLINE EZombieState GetZombieState() { return ZombieState; }

    // ���� ��ġ ������Ʈ�� ��ȯ
    FORCEINLINE UServerLocationComponent* GetServerLocationComponent() { return ServerLocationComp; }

protected:
    // ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
    virtual void BeginPlay() override;

    // ��Ʈ��ũ ������ ���� �Ӽ� ���� �Լ�
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // ������Ʈ�� �ʱ�ȭ�� �� ȣ��Ǵ� �Լ�
    virtual void PostInitializeComponents() override;

    // �ǰ� �� ��� ó�� ���� �Լ�
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // ��� ó�� �Լ�
    void HandleDeath();

    // �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
    virtual void Tick(float DeltaTime) override;

    // �÷��̾ �þ� ���� �߰����� �� ȣ��Ǵ� �Լ�
    UFUNCTION()
        void OnSeePlayer(APawn* Pawn);

    // Ÿ���� �þ� ���� �ִ��� Ȯ���ϴ� �Լ�
    UFUNCTION()
        bool IsTargetInSight(APawn* Target);

    // ������ ������ϴ� �Լ�
    UFUNCTION()
        void RestartPatrol();

    // ������ ����� �� ȣ��Ǵ� �Լ�
    UFUNCTION()
        void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

    // Ÿ���� �����ϴ� �Լ�
    UFUNCTION()
        virtual void Attack(APawn* Target);

    // Ÿ�ٿ��� �������� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        virtual void ApplyDamageToTarget();

    // ��Ʈ ���׼� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void HitReactEnd();

    // ��� �ִϸ��̼� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void DeathEnd();

    // ���� �����
    UPROPERTY(EditAnywhere)
        float Damage;

    // ������ ü��
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Health")
        float Health;

    // ���� ���� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Combat")
        float AttackRange;

    // ���� �ݰ�
    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float PatrolRadius;

    // ��������� �ð�
    UPROPERTY(EditDefaultsOnly, Category = "Zombie|AI")
        float TimeToSleep;

    // ���� ���� ���� Ÿ��
    UPROPERTY(Replicated)
        APawn* CurrentTarget;

    // ���� ����
    UPROPERTY(Replicated)
        EZombieState ZombieState;

    // ���� ��� �ð� (��)
    float AttackCooldown = 5.0f;

    // ��Ʈ ���׼� ���� Ÿ�̸� �ڵ�
    FTimerHandle TimerHandle_HitReactEnd;

private:
    // ���� ���� ����Ʈ�� ��� �Լ�
    FVector GetRandomPatrolPoint();

    // ���� ����� �Լ�
    void WakeUp();

    // ���� �ٽ� ������ �Լ�
    void GoBackToSleep();

    // ������ ���� �ð�
    UPROPERTY()
        float LastAttackTime;

    // PawnSensingComponent
    UPROPERTY(VisibleAnywhere, Category = "Zombie|Components")
        UPawnSensingComponent* PawnSensingComp;

    // ���� ����Ʈ
    UPROPERTY(Replicated)
        FVector PatrolPoint;

    // ���������� �÷��̾ ������ �ð�
    UPROPERTY()
        float LastSeenTime;

    // ���� ��ġ ������Ʈ
    UPROPERTY()
        class UServerLocationComponent* ServerLocationComp;

    // ���ڱ� Ÿ�̸� �ڵ�
    UPROPERTY()
        FTimerHandle SleepTimerHandle;
};
