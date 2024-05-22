#include "MachineGun.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"

// ������: ������� �⺻ �Ӽ��� �ʱ�ȭ�մϴ�.
AMachineGun::AMachineGun()
{
    // źâ �ִ� �뷮 ����
    Capacity = 30;
    // ���� ź�� ���� �ִ� �뷮���� �ʱ�ȭ
    CurrentAmmo = Capacity;
    // ���� ������ ����
    Damage = 30;

    // �߻� �ӵ� ���� (0.1�� �������� �߻�)
    FireRate = 0.1f;
}

// �߻� �Լ�: ���⸦ �߻��մϴ�.
void AMachineGun::Fire()
{
    // �߻簡 �������� Ȯ���մϴ�.
    if (CanFire()) {
        return;
    }

    // �θ� Ŭ������ �߻� �Լ� ȣ��
    Super::Fire();

    // ���� ź�� �� ����
    CurrentAmmo--;

    // HUD�� ź�� ������ ������Ʈ
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}

// �߻� ���� �Լ�: �߻� ���¸� �����մϴ�.
void AMachineGun::FireEnd()
{
    // �θ� Ŭ������ �߻� ���� �Լ� ȣ��
    Super::FireEnd();

    // �߻� Ÿ�̸Ӹ� �ʱ�ȭ�Ͽ� �� �̻� �߻����� �ʵ��� �մϴ�.
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

// �߻� �ֱ� ���� �Լ�: �߻� �ֱⰡ ����� �� ȣ��˴ϴ�.
void AMachineGun::ShootEnd()
{
    // ���� ���¸� None���� ����
    WeaponState = EWeaponState::None;

    // �߻� Ÿ�̸Ӹ� �����Ͽ� ���� �ð� �Ŀ� �ٽ� �߻��� �� �ֵ��� �մϴ�.
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AMachineGun::Fire, FireRate, false, FireRate);
}

// �߻簡 �������� Ȯ���ϴ� �Լ�
bool AMachineGun::CanFire()
{
    // ���� ������ Ȯ���Ͽ� �߻簡 �������� ���θ� ��ȯ
    return Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetMachineGunAmmo() < 1 || CurrentAmmo < 1 || WeaponState == EWeaponState::Reload;
}

// ���� ���� Ÿ���� ��ȯ�ϴ� �Լ�
EWeaponType AMachineGun::GetCurrentWeaponType()
{
    return EWeaponType::MachineGun;
}

// ������ �Լ�: źâ�� �������մϴ�.
void AMachineGun::PerformReload()
{
    // �θ� Ŭ������ ������ �Լ� ȣ��
    Super::PerformReload();

    // ĳ���� �Ǵ� �κ��丮 ������Ʈ�� ���� ��� ����
    if (Character == nullptr || Character->InventoryComp == nullptr || WeaponState != EWeaponState::None)
    {
        return;
    }

    // �������� ź�� �� ���
    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());

    // ���� ź�� �� ������Ʈ
    CurrentAmmo += AmmoToReload;

    // �κ��丮�� ź�� �� ����
    Character->InventoryComp->SetMachineGunAmmo(Character->InventoryComp->GetMachineGunAmmo() - AmmoToReload);

    // HUD�� ź�� ������ ������Ʈ
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}

// ���⸦ ��ü�ϴ� �Լ�
void AMachineGun::ChangeWeapon()
{
    // �θ� Ŭ������ ���� ��ü �Լ� ȣ��
    Super::ChangeWeapon();

    // HUD�� ź�� ������ ������Ʈ
    Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
}
