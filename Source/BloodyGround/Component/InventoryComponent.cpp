#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// 생성자
UInventoryComponent::UInventoryComponent()
{
    // 컴포넌트의 틱 비활성화
    PrimaryComponentTick.bCanEverTick = false;

    // 네트워크 복제 활성화
    SetIsReplicatedByDefault(true);
}

// 컴포넌트가 게임 시작 시 초기화되는 함수
void UInventoryComponent::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();
}

// 새로운 무기를 인벤토리에 추가
void UInventoryComponent::AddWeapon(ABaseWeapon* NewWeapon)
{
    // 새로운 무기가 유효하고, 이미 인벤토리에 없을 경우 추가
    if (NewWeapon && !Weapons.Contains(NewWeapon))
    {
        Weapons.Add(NewWeapon);
    }
}

// 인벤토리에서 무기를 제거
void UInventoryComponent::RemoveWeapon(ABaseWeapon* WeaponToRemove)
{
    // 제거할 무기가 유효하고, 인벤토리에 있을 경우 제거
    if (WeaponToRemove && Weapons.Contains(WeaponToRemove))
    {
        Weapons.Remove(WeaponToRemove);
    }
}

// 초기 무기를 설정
void UInventoryComponent::SetInitWeapon(ABaseWeapon* Weapon)
{
    CurrentWeapon = Weapon;
}

// 인벤토리에 있는 모든 무기를 파괴
void UInventoryComponent::DestroyAllWeapons()
{
    // 모든 무기를 순회하며 파괴
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

// 현재 무기를 다음 무기로 변경
void UInventoryComponent::ChangeWeapon()
{
    // 무기가 한 개 이하일 경우 전환할 수 없음
    if (Weapons.Num() <= 1)
    {
        return;
    }

    // 현재 무기 인덱스 찾기
    int32 CurrentWeaponIndex = Weapons.IndexOfByKey(CurrentWeapon);
    // 다음 무기 인덱스 계산
    int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();

    // 새 무기 설정
    ABaseWeapon* NewWeapon = Weapons[NextWeaponIndex];

    // 현재 무기 숨기기
    if (CurrentWeapon)
    {
        CurrentWeapon->WeaponState = EWeaponState::None;
        CurrentWeapon->SetActorHiddenInGame(true);
    }

    // 새 무기 보이기
    CurrentWeapon = NewWeapon;
    CurrentWeapon->SetActorHiddenInGame(false);
    if (CurrentWeapon)
    {
        CurrentWeapon->ChangeWeapon();
    }
}

// 현재 무기를 반환
ABaseWeapon* UInventoryComponent::GetCurrentWeapon() const
{
    return CurrentWeapon;
}

// 권총 탄약 설정
void UInventoryComponent::SetPistolAmmo(int32 NewAmmo)
{
    PistolAmmo = NewAmmo;
}

// 기관총 탄약 설정
void UInventoryComponent::SetMachineGunAmmo(int32 NewAmmo)
{
    MachineGunAmmo = NewAmmo;
}

// 네트워크 복제를 위한 속성 설정
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // 부모 클래스의 네트워크 복제 설정 호출
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 인벤토리 컴포넌트의 속성들 네트워크 복제 설정
    DOREPLIFETIME(UInventoryComponent, Weapons);
    DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
    DOREPLIFETIME(UInventoryComponent, PistolAmmo);
    DOREPLIFETIME(UInventoryComponent, MachineGunAmmo);
}
