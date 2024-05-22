#include "MachineGun.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"

// 생성자: 기관총의 기본 속성을 초기화합니다.
AMachineGun::AMachineGun()
{
    // 탄창 최대 용량 설정
    Capacity = 30;
    // 현재 탄약 수를 최대 용량으로 초기화
    CurrentAmmo = Capacity;
    // 무기 데미지 설정
    Damage = 30;

    // 발사 속도 설정 (0.1초 간격으로 발사)
    FireRate = 0.1f;
}

// 발사 함수: 무기를 발사합니다.
void AMachineGun::Fire()
{
    // 발사가 가능한지 확인합니다.
    if (CanFire()) {
        return;
    }

    // 부모 클래스의 발사 함수 호출
    Super::Fire();

    // 현재 탄약 수 감소
    CurrentAmmo--;

    // HUD에 탄약 정보를 업데이트
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}

// 발사 종료 함수: 발사 상태를 종료합니다.
void AMachineGun::FireEnd()
{
    // 부모 클래스의 발사 종료 함수 호출
    Super::FireEnd();

    // 발사 타이머를 초기화하여 더 이상 발사하지 않도록 합니다.
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

// 발사 주기 종료 함수: 발사 주기가 종료될 때 호출됩니다.
void AMachineGun::ShootEnd()
{
    // 무기 상태를 None으로 설정
    WeaponState = EWeaponState::None;

    // 발사 타이머를 설정하여 일정 시간 후에 다시 발사할 수 있도록 합니다.
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AMachineGun::Fire, FireRate, false, FireRate);
}

// 발사가 가능한지 확인하는 함수
bool AMachineGun::CanFire()
{
    // 여러 조건을 확인하여 발사가 가능한지 여부를 반환
    return Character == nullptr || Character->InventoryComp == nullptr || Character->InventoryComp->GetMachineGunAmmo() < 1 || CurrentAmmo < 1 || WeaponState == EWeaponState::Reload;
}

// 현재 무기 타입을 반환하는 함수
EWeaponType AMachineGun::GetCurrentWeaponType()
{
    return EWeaponType::MachineGun;
}

// 재장전 함수: 탄창을 재장전합니다.
void AMachineGun::PerformReload()
{
    // 부모 클래스의 재장전 함수 호출
    Super::PerformReload();

    // 캐릭터 또는 인벤토리 컴포넌트가 없는 경우 리턴
    if (Character == nullptr || Character->InventoryComp == nullptr || WeaponState != EWeaponState::None)
    {
        return;
    }

    // 재장전할 탄약 수 계산
    int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());

    // 현재 탄약 수 업데이트
    CurrentAmmo += AmmoToReload;

    // 인벤토리의 탄약 수 감소
    Character->InventoryComp->SetMachineGunAmmo(Character->InventoryComp->GetMachineGunAmmo() - AmmoToReload);

    // HUD에 탄약 정보를 업데이트
    if (Character->GetInGameHUD())
    {
        Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
    }
}

// 무기를 교체하는 함수
void AMachineGun::ChangeWeapon()
{
    // 부모 클래스의 무기 교체 함수 호출
    Super::ChangeWeapon();

    // HUD에 탄약 정보를 업데이트
    Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
}
