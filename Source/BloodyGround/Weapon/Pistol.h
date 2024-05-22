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
    // Pistol 클래스 생성자: 권총의 기본 속성을 초기화합니다.
    APistol();

    // 무기를 발사하는 함수
    UFUNCTION()
        virtual void Fire() override;

    // 현재 무기 타입을 반환하는 함수
    virtual EWeaponType GetCurrentWeaponType() override;

    // 탄창을 재장전하는 함수
    virtual void PerformReload() override;

    // 무기를 교체하는 함수
    virtual void ChangeWeapon() override;

protected:

    // 발사가 가능한지 확인하는 함수
    UFUNCTION()
        virtual bool CanFire() override;

private:

};
