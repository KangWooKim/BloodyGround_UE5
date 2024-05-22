#include "Pistol.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"

// 생성자: 권총의 기본 속성을 초기화합니다.
APistol::APistol()
{
    // 탄창 최대 용량 설정
    Capacity = 10;
    // 현재 탄약 수를 최대 용량으로 초기화
    CurrentAmmo = Capacity;
    // 무기 데미지 설정
    Damage = 10;
}

// 발사 함수: 무기를 발사합니다.
void APistol::Fire()
{
    // 발사가 가능한지 확인합니다.
    if (CanFire()) return;

    // 부모 클래스의 발사 함수 호출
    Super::Fire();

    // 현재 탄약 수 감소
    CurrentAmmo--;

    // HUD에 탄약 정보를 업데이트
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
    }
}

// 발사가 가능한지 확인하는 함수
bool APistol::CanFire()
{
    // 여러 조건을 확인하여 발사가 가능한지 여부를 반환
    return WeaponState == EWeaponState::Reload || Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetPistolAmmo() < 1 || CurrentAmmo < 1;
}

// 현재 무기 타입을 반환하는 함수
EWeaponType APistol::GetCurrentWeaponType()
{
    return EWeaponType::Pistol;
}

// 재장전 함수: 탄창을 재장전합니다.
void APistol::PerformReload()
{
    // 부모 클래스의 재장전 함수 호출
    Super::PerformReload();

    // 캐릭터 또는 인벤토리 컴포넌트가 없는 경우 리턴
    if (Character == nullptr || Character->InventoryComp == nullptr || WeaponState != EWeaponState::None)
    {
        return;
    }

    // 재장전할 탄약 수 계산
    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetPistolAmmo());

    // 현재 탄약 수 업데이트
    CurrentAmmo += AmmoToReload;

    // 인벤토리의 탄약 수 감소
    Character->InventoryComp->SetPistolAmmo(Character->InventoryComp->GetPistolAmmo() - AmmoToReload);

    // HUD에 탄약 정보를 업데이트
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
    }
}

// 무기를 교체하는 함수
void APistol::ChangeWeapon()
{
    // 부모 클래스의 무기 교체 함수 호출
    Super::ChangeWeapon();

    // HUD에 탄약 정보를 업데이트
    Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
}
