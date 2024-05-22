// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAnimInstance.h"
#include "BaseWeapon.h"

// 애니메이션 업데이트 함수: 매 프레임마다 호출되며 애니메이션 상태를 갱신합니다.
// @param DeltaSeconds 마지막 틱 이후 경과된 시간
void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    // 부모 클래스의 NativeUpdateAnimation 함수 호출
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 현재 소유한 무기를 가져옵니다.
    ABaseWeapon* Weapon = Cast<ABaseWeapon>(TryGetPawnOwner());

    // 무기가 유효한지 확인합니다.
    if (Weapon)
    {
        // 무기의 상태를 가져와서 애니메이션 상태를 갱신합니다.
        WeaponState = Weapon->GetWeaponState();
    }
}
