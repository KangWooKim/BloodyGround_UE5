// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAnimInstance.h"
#include "BaseWeapon.h"

// �ִϸ��̼� ������Ʈ �Լ�: �� �����Ӹ��� ȣ��Ǹ� �ִϸ��̼� ���¸� �����մϴ�.
// @param DeltaSeconds ������ ƽ ���� ����� �ð�
void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    // �θ� Ŭ������ NativeUpdateAnimation �Լ� ȣ��
    Super::NativeUpdateAnimation(DeltaSeconds);

    // ���� ������ ���⸦ �����ɴϴ�.
    ABaseWeapon* Weapon = Cast<ABaseWeapon>(TryGetPawnOwner());

    // ���Ⱑ ��ȿ���� Ȯ���մϴ�.
    if (Weapon)
    {
        // ������ ���¸� �����ͼ� �ִϸ��̼� ���¸� �����մϴ�.
        WeaponState = Weapon->GetWeaponState();
    }
}
