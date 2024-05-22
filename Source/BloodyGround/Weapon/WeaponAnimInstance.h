// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseWeapon.h"
#include "WeaponAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class BLOODYGROUND_API UWeaponAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:

    // 애니메이션 업데이트 시 호출되는 함수
    // @param DeltaSeconds 마지막 틱 이후 경과된 시간
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

    // 현재 무기 상태를 나타내는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponState")
        EWeaponState WeaponState;
};
