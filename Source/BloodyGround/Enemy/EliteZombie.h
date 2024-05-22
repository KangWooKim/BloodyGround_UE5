// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseZombie.h"
#include "EliteZombie.generated.h"

// ���� �λ� ���¸� ��Ÿ���� ������
UENUM(BlueprintType)
enum class EZombieInjuryState : uint8
{
    None    UMETA(DisplayName = "None"),       // �λ� ����
    Injured  UMETA(DisplayName = "Injured"),   // �λ� ����
    Down  UMETA(DisplayName = "Down"),         // ������ ����
    Stand  UMETA(DisplayName = "Stand")        // �Ͼ�� ����
};

/**
 * ����Ʈ ���� Ŭ����
 */
UCLASS()
class BLOODYGROUND_API AEliteZombie : public ABaseZombie
{
    GENERATED_BODY()

public:

    // ������
    AEliteZombie();

    // ���� ���� �λ� ���¸� ��ȯ
    FORCEINLINE EZombieInjuryState GetZombieInjuryState() { return ZombieInjuryState; }

    // �������̵�� ������ ó�� �Լ�
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // ���� �����߸��� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void GetDown();

    // ���� ������ ���¸� ������ �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void DownEnd();

    // ���� �Ͼ�� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void StandUp();

    // ���� �Ͼ�� ���¸� ������ �Լ�
    UFUNCTION(BlueprintCallable, Category = "Zombie|Combat")
        void StandUpEnd();

    // Ÿ���� �����ϴ� �Լ� (�������̵�)
    virtual void Attack(APawn* Target) override;

    // Ÿ�ٿ��� �������� �����ϴ� �Լ� (�������̵�)
    virtual void ApplyDamageToTarget() override;

    // �Ѱ��� �޾��� ���� ó�� �Լ� (�������̵�)
    virtual void TakeShot(FHitResultData HitResult, float WeaponDamage) override;

protected:

    // ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
    virtual void BeginPlay() override;

    // ��Ʈ��ũ ������ ���� �Ӽ� ���� �Լ�
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // �ٸ��� ���� ������ ����
    UPROPERTY(Replicated)
        float LegDamageAccumulated;

private:

    // ���� �λ� ����
    UPROPERTY(Replicated)
        EZombieInjuryState ZombieInjuryState;
};
