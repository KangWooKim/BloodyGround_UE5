# BaseWeapon

### `ABaseWeapon()`
이 생성자는 `ABaseWeapon` 클래스의 초기 설정을 담당합니다. 스켈레탈 메시 컴포넌트를 초기화하고, 복제 가능 여부를 설정합니다. 또한, 무기 메시의 충돌 응답을 구성하고, 소음 발생 컴포넌트를 추가합니다. 이 액터는 무기 시스템의 기반을 제공하며, 게임 내에서 플레이어가 사용할 수 있는 다양한 무기의 기능을 구현하는 데 사용됩니다.

### `BeginPlay()`
게임 플레이가 시작될 때 호출되며, 무기의 초기 상태를 설정하고 무기의 소유자를 캐릭터로 캐스팅합니다.

### `GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
네트워크 복제를 위해 복제되어야 할 변수들을 설정합니다. 무기 상태와 현재 탄약 수가 포함됩니다.

### `ChangeWeapon()`
현재 무기를 변경하는 메서드입니다. 구체적인 로직은 구현되지 않았으나, 무기 교체 로직의 틀을 제공합니다.

### `Tick(float DeltaTime)`
매 프레임마다 호출되는 이 메서드는 현재는 활성화되지 않았습니다. 필요에 따라 무기의 상태나 다른 요소를 매 프레임 업데이트하는 로직을 추가할 수 있습니다.

### `Fire()`
무기 발사 로직을 구현합니다. 네트워크 권한에 따라 애니메이션 재생과 소음 발생, 타격 검사 등을 수행합니다. 또한, 발사 위치와 방향을 기반으로 선형 트레이스를 수행하여 목표를 검사하고 피해를 적용합니다.

### `ServerCheckHit_Implementation(FHitResult ClientHitResult, float HitTime, FVector StartLocation, FVector EndDirection)`
클라이언트에서 발생한 히트를 서버에서 검증하는 메서드입니다. 서버 위치 컴포넌트를 사용하여 정확한 시간과 위치에서의 적중을 계산합니다.

### `WeaponNoise_Implementation(float Loudness)`
발사 시 또는 재장전 시 발생하는 소음을 생성합니다. 이 메서드는 캐릭터가 소리를 발생시켜 주변 NPC에게 들릴 수 있게 합니다.

### `CanFire()`
무기가 발사 가능한 상태인지 확인하는 메서드입니다. 현재는 항상 `true`를 반환합니다.

### `FireEnd()`
발사가 끝났을 때 호출되어 무기 상태를 업데이트합니다.

### `Reload()`
무기의 재장전 로직을 구현합니다. 클라이언트와 서버에서의 재장전 요청을 처리합니다.

### `PerformReload()`
실제 재장전을 수행하는 메서드입니다. 재장전 애니메이션을 재생하고, 소음을 발생시킵니다.

### `ServerReload_Implementation()`
서버에서 재장전을 수행하는 메서드입니다. 실제 게임 환경에서는 이 메서드를 통해 서버에서 무기의 재장전을 처리합니다.

### `MulticastPlayReloadAnimation_Implementation()`
모든 클라이언트에서 재장전 애니메이션을 재생합니다.

### `ServerPlayFireAnimation_Implementation()`
서버에서 발사 애니메이션과 소음을 처리합니다.

### `MulticastPlayFireAnimation_Implementation()`
모든 클라이언트에서 발사 애니메이션을 재생합니다.

# MachineGun

### `AMachineGun()`
이 생성자는 `AMachineGun` 클래스의 초기 설정을 담당합니다. 탄약 용량, 현재 탄약 수, 데미지, 그리고 발사 속도를 초기화합니다.

### `Fire()`
기계총 발사 로직을 구현합니다. 발사 가능한 상태일 때 현재 탄약을 감소시키고, 관련 HUD를 업데이트하여 현재 탄약 상태를 반영합니다. 이 메서드는 플레이어가 발사 버튼을 누를 때 호출되며, 발사 가능 상태를 검사한 후 슈퍼 클래스의 `Fire` 메서드를 호출합니다.

### `FireEnd()`
발사가 끝났을 때 호출되며, 발사와 관련된 타이머를 초기화합니다. 이는 연속 발사를 중단하거나 발사 로직의 종료를 처리하는 데 사용됩니다.

### `ShootEnd()`
발사를 지속적으로 수행하기 위해 타이머를 설정하는 메서드입니다. 이 타이머는 지정된 발사 속도(`FireRate`)에 따라 `Fire` 메서드를 주기적으로 호출합니다.

### `CanFire()`
기계총이 발사 가능한 상태인지 확인합니다. 현재 탄약 수, 무기 상태, 캐릭터의 인벤토리 컴포넌트를 검사하여 발사 가능 여부를 결정합니다.

### `GetCurrentWeaponType()`
현재 무기의 타입을 반환합니다. 이 경우 `MachineGun`으로 설정됩니다.

### `PerformReload()`
재장전 로직을 구현합니다. 재장전 시 필요한 탄약 수를 계산하고, 캐릭터의 탄약 상태를 업데이트합니다. 이 메서드는 캐릭터의 인벤토리에서 사용 가능한 탄약을 현재 무기에 적용하며, 관련 HUD 정보도 업데이트합니다.

### `ChangeWeapon()`
무기 변경 시 호출되며, 관련 HUD 정보를 업데이트하여 새로운 무기의 탄약 상태를 반영합니다. 이는 플레이어가 무기를 교체할 때 현재 탄약 상태를 즉각적으로 표시하기 위해 사용됩니다.

# Pistol

### `APistol()`
이 생성자는 `APistol` 클래스의 초기 설정을 담당합니다. 권총의 탄약 용량, 현재 탄약 수, 그리고 데미지를 초기화합니다.

### `Fire()`
권총 발사 로직을 구현합니다. 발사 가능한 상태일 때 현재 탄약을 감소시키고, 관련 HUD를 업데이트하여 현재 탄약 상태를 반영합니다. 이 메서드는 플레이어가 발사 버튼을 누를 때 호출되며, 발사 가능 상태를 검사한 후 슈퍼 클래스의 `Fire` 메서드를 호출합니다.

### `CanFire()`
권총이 발사 가능한 상태인지 확인합니다. 무기 상태, 캐릭터의 존재 여부, 캐릭터의 인벤토리 상태, 현재 탄약과 전체 탄약을 검사하여 발사 가능 여부를 결정합니다.

### `GetCurrentWeaponType()`
현재 무기의 타입을 반환합니다. 이 경우 `Pistol`로 설정됩니다.

### `PerformReload()`
재장전 로직을 구현합니다. 필요한 탄약 수를 계산하고, 캐릭터의 탄약 상태를 업데이트합니다. 이 메서드는 캐릭터의 인벤토리에서 사용 가능한 탄약을 현재 무기에 적용하며, 관련 HUD 정보도 업데이트합니다.

### `ChangeWeapon()`
무기 변경 시 호출되며, 관련 HUD 정보를 업데이트하여 새로운 무기의 탄약 상태를 반영합니다. 이는 플레이어가 무기를 교체할 때 현재 탄약 상태를 즉각적으로 표시하기 위해 사용됩니다.