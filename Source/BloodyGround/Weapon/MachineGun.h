// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MachineGun.generated.h"

/**
 * AMachineGun Ŭ������ ABaseWeapon�� ��ӹ޾� ������� ����� �����մϴ�.
 */
UCLASS()
class BLOODYGROUND_API AMachineGun : public ABaseWeapon
{
    GENERATED_BODY()

public:

    // ������: ������� �⺻ �Ӽ��� �ʱ�ȭ�մϴ�.
    AMachineGun();

    // ���⸦ �߻��ϴ� �Լ�
    UFUNCTION()
        virtual void Fire() override;

    // ���� ���� Ÿ���� ��ȯ�ϴ� �Լ�
    virtual EWeaponType GetCurrentWeaponType() override;

    // źâ�� �������ϴ� �Լ�
    virtual void PerformReload() override;

    // ���⸦ ��ü�ϴ� �Լ�
    virtual void ChangeWeapon() override;

protected:

    // �߻簡 �������� Ȯ���ϴ� �Լ�
    UFUNCTION()
        virtual bool CanFire() override;

    // �߻� ���� �Լ�
    virtual void FireEnd() override;

    // �߻� �ֱⰡ ����� �� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintCallable)
        void ShootEnd();

private:

    // ���� �߻縦 ���� Ÿ�̸� �ڵ�
    FTimerHandle FireTimerHandle;

    // �߻� ����
    float FireRate;
};
