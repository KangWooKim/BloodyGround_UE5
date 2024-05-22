#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// ������
UInventoryComponent::UInventoryComponent()
{
    // ������Ʈ�� ƽ ��Ȱ��ȭ
    PrimaryComponentTick.bCanEverTick = false;

    // ��Ʈ��ũ ���� Ȱ��ȭ
    SetIsReplicatedByDefault(true);
}

// ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
void UInventoryComponent::BeginPlay()
{
    // �θ� Ŭ������ BeginPlay ȣ��
    Super::BeginPlay();
}

// ���ο� ���⸦ �κ��丮�� �߰�
void UInventoryComponent::AddWeapon(ABaseWeapon* NewWeapon)
{
    // ���ο� ���Ⱑ ��ȿ�ϰ�, �̹� �κ��丮�� ���� ��� �߰�
    if (NewWeapon && !Weapons.Contains(NewWeapon))
    {
        Weapons.Add(NewWeapon);
    }
}

// �κ��丮���� ���⸦ ����
void UInventoryComponent::RemoveWeapon(ABaseWeapon* WeaponToRemove)
{
    // ������ ���Ⱑ ��ȿ�ϰ�, �κ��丮�� ���� ��� ����
    if (WeaponToRemove && Weapons.Contains(WeaponToRemove))
    {
        Weapons.Remove(WeaponToRemove);
    }
}

// �ʱ� ���⸦ ����
void UInventoryComponent::SetInitWeapon(ABaseWeapon* Weapon)
{
    CurrentWeapon = Weapon;
}

// �κ��丮�� �ִ� ��� ���⸦ �ı�
void UInventoryComponent::DestroyAllWeapons()
{
    // ��� ���⸦ ��ȸ�ϸ� �ı�
    for (int32 index = 0; index < Weapons.Num(); index++)
    {
        if (Weapons[index])
        {
            Weapons[index]->Destroy();
        }
    }
    Weapons.Empty();
    CurrentWeapon = nullptr;
}

// ���� ���⸦ ���� ����� ����
void UInventoryComponent::ChangeWeapon()
{
    // ���Ⱑ �� �� ������ ��� ��ȯ�� �� ����
    if (Weapons.Num() <= 1)
    {
        return;
    }

    // ���� ���� �ε��� ã��
    int32 CurrentWeaponIndex = Weapons.IndexOfByKey(CurrentWeapon);
    // ���� ���� �ε��� ���
    int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

    // �� ���� ����
    ABaseWeapon* NewWeapon = Weapons[NextWeaponIndex];

    // ���� ���� �����
    if (CurrentWeapon)
    {
        CurrentWeapon->WeaponState = EWeaponState::None;
        CurrentWeapon->SetActorHiddenInGame(true);
    }

    // �� ���� ���̱�
    CurrentWeapon = NewWeapon;
    CurrentWeapon->SetActorHiddenInGame(false);
    if (CurrentWeapon)
    {
        CurrentWeapon->ChangeWeapon();
    }
}

// ���� ���⸦ ��ȯ
ABaseWeapon* UInventoryComponent::GetCurrentWeapon() const
{
    return CurrentWeapon;
}

// ���� ź�� ����
void UInventoryComponent::SetPistolAmmo(int32 NewAmmo)
{
    PistolAmmo = NewAmmo;
}

// ����� ź�� ����
void UInventoryComponent::SetMachineGunAmmo(int32 NewAmmo)
{
    MachineGunAmmo = NewAmmo;
}

// ��Ʈ��ũ ������ ���� �Ӽ� ����
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // �θ� Ŭ������ ��Ʈ��ũ ���� ���� ȣ��
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // �κ��丮 ������Ʈ�� �Ӽ��� ��Ʈ��ũ ���� ����
    DOREPLIFETIME(UInventoryComponent, Weapons);
    DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
    DOREPLIFETIME(UInventoryComponent, PistolAmmo);
    DOREPLIFETIME(UInventoryComponent, MachineGunAmmo);
}
