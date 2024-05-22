#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Pistol.generated.h"

UCLASS()
class BLOODYGROUND_API APistol : public ABaseWeapon
{
    GENERATED_BODY()

public:
    // Pistol Ŭ���� ������: ������ �⺻ �Ӽ��� �ʱ�ȭ�մϴ�.
    APistol();

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

private:

};
