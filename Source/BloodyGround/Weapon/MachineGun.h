// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MachineGun.generated.h"

/**
 * AMachineGun 클래스는 ABaseWeapon을 상속받아 기관총의 기능을 구현합니다.
 */
UCLASS()
class BLOODYGROUND_API AMachineGun : public ABaseWeapon
{
    GENERATED_BODY()

public:

    // 생성자: 기관총의 기본 속성을 초기화합니다.
    AMachineGun();

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

    // 발사 종료 함수
    virtual void FireEnd() override;

    // 발사 주기가 종료될 때 호출되는 함수
    UFUNCTION(BlueprintCallable)
        void ShootEnd();

private:

    // 연속 발사를 위한 타이머 핸들
    FTimerHandle FireTimerHandle;

    // 발사 간격
    float FireRate;
};
