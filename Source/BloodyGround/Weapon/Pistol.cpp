#include "Pistol.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"

// ������: ������ �⺻ �Ӽ��� �ʱ�ȭ�մϴ�.
APistol::APistol()
{
    // źâ �ִ� �뷮 ����
    Capacity = 10;
    // ���� ź�� ���� �ִ� �뷮���� �ʱ�ȭ
    CurrentAmmo = Capacity;
    // ���� ������ ����
    Damage = 10;
}

// �߻� �Լ�: ���⸦ �߻��մϴ�.
void APistol::Fire()
{
    // �߻簡 �������� Ȯ���մϴ�.
    if (CanFire()) return;

    // �θ� Ŭ������ �߻� �Լ� ȣ��
    Super::Fire();

    // ���� ź�� �� ����
    CurrentAmmo--;

    // HUD�� ź�� ������ ������Ʈ
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
    }
}

// �߻簡 �������� Ȯ���ϴ� �Լ�
bool APistol::CanFire()
{
    // ���� ������ Ȯ���Ͽ� �߻簡 �������� ���θ� ��ȯ
    return WeaponState == EWeaponState::Reload || Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetPistolAmmo() < 1 || CurrentAmmo < 1;
}

// ���� ���� Ÿ���� ��ȯ�ϴ� �Լ�
EWeaponType APistol::GetCurrentWeaponType()
{
    return EWeaponType::Pistol;
}

// ������ �Լ�: źâ�� �������մϴ�.
void APistol::PerformReload()
{
    // �θ� Ŭ������ ������ �Լ� ȣ��
    Super::PerformReload();

    // ĳ���� �Ǵ� �κ��丮 ������Ʈ�� ���� ��� ����
    if (Character == nullptr || Character->InventoryComp == nullptr || WeaponState != EWeaponState::None)
    {
        return;
    }

    // �������� ź�� �� ���
    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetPistolAmmo());

    // ���� ź�� �� ������Ʈ
    CurrentAmmo += AmmoToReload;

    // �κ��丮�� ź�� �� ����
    Character->InventoryComp->SetPistolAmmo(Character->InventoryComp->GetPistolAmmo() - AmmoToReload);

    // HUD�� ź�� ������ ������Ʈ
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
    }
}

// ���⸦ ��ü�ϴ� �Լ�
void APistol::ChangeWeapon()
{
    // �θ� Ŭ������ ���� ��ü �Լ� ȣ��
    Super::ChangeWeapon();

    // HUD�� ź�� ������ ������Ʈ
    Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
}
