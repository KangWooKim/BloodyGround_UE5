# BattleComponent

### `UBattleComponent()`
이 메서드는 `UBattleComponent` 클래스의 생성자로서 컴포넌트의 초기 설정을 담당합니다. 컴포넌트가 매 프레임마다 업데이트되지 않도록 설정하고, 조준 상태를 비활성화한 상태로 초기화합니다. 또한 네트워크 복제를 활성화하여 멀티플레이 게임에서도 이 컴포넌트의 정보가 동기화될 수 있도록 설정합니다.

### `GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const`
이 메서드는 네트워크 복제 시 필요한 속성들을 설정합니다. `bIsAiming` 변수가 네트워크를 통해 동기화되도록 설정하여, 다른 플레이어들도 해당 캐릭터의 조준 상태를 볼 수 있습니다.

### `BeginPlay()`
게임 시작 시 호출되어, 기본적인 시작 로직을 처리합니다. 이 메서드는 주로 컴포넌트 초기화 후 필요한 추가 설정을 수행하는 데 사용됩니다.

### `TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
이 컴포넌트가 업데이트될 필요가 있는 경우, 주어진 틱 유형과 시간에 따라 호출됩니다. 기본적으로는 업데이트가 비활성화되어 있지만, 필요에 따라 활성화하여 사용할 수 있습니다.

### `StartAiming()`
조준을 시작하는 메서드로, `bIsAiming` 변수를 `true`로 설정합니다. 이는 캐릭터가 무기를 조준 모드로 전환하고 있음을 나타내며, 이에 따른 애니메이션 변경이나 게임플레이 로직에 영향을 줄 수 있습니다.

### `StopAiming()`
조준을 중지하는 메서드로, `bIsAiming` 변수를 `false`로 설정합니다. 캐릭터가 조준 모드에서 일반 모드로 전환하고 있음을 나타내며, 이는 또한 애니메이션과 게임플레이 로직에 영향을 미칩니다.

# InventoryComponent

### `UInventoryComponent()`
이 생성자는 `UInventoryComponent` 클래스의 초기 설정을 담당합니다. 이 컴포넌트는 매 프레임마다 업데이트되지 않도록 설정되어 있으며, 네트워크 복제를 활성화하여 멀티플레이 게임에서도 인벤토리 정보가 동기화될 수 있도록 합니다.

### `BeginPlay()`
게임 시작 시 호출되어, 컴포넌트에 필요한 초기화 작업을 수행합니다.

### `AddWeapon(ABaseWeapon* NewWeapon)`
새로운 무기를 인벤토리에 추가합니다. 이 메서드는 무기가 인벤토리에 이미 존재하지 않는 경우에만 무기를 추가합니다.

### `RemoveWeapon(ABaseWeapon* WeaponToRemove)`
인벤토리에서 특정 무기를 제거합니다. 제거하려는 무기가 인벤토리에 존재하는 경우에만 작업을 수행합니다.

### `SetInitWeapon(ABaseWeapon* Weapon)`
초기 무기를 설정합니다. 이 메서드는 캐릭터가 게임을 시작할 때 초기 무기를 지정하는 데 사용됩니다.

### `DestroyAllWeapons()`
인벤토리에 있는 모든 무기를 파괴합니다. 이 메서드는 캐릭터의 사망 시 무기를 제거하는 등의 상황에서 사용됩니다.

### `ChangeWeapon()`
인벤토리 내의 다음 무기로 전환합니다. 현재 무기를 숨기고, 다음 순서의 무기를 활성화합니다. 이 메서드는 플레이어가 무기 전환 버튼을 누를 때 호출됩니다.

### `GetCurrentWeapon()`
현재 선택되어 있는 무기를 반환합니다. 이 메서드는 다양한 게임 로직에서 현재 활성화된 무기 정보를 얻기 위해 사용됩니다.

### `SetPistolAmmo(int32 NewAmmo)` 및 `SetMachineGunAmmo(int32 NewAmmo)`
각각 권총과 머신건의 탄약 수를 설정합니다. 이 메서드들은 무기를 변경하거나 탄약을 재보급할 때 사용됩니다.

### `GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
네트워크 복제 시 필요한 속성들을 설정합니다. 인벤토리 내의 무기 목록, 현재 무기, 그리고 각 무기의 탄약 수가 네트워크를 통해 동기화되도록 설정합니다. 이는 다른 플레이어와의 게임 플레이 중 인벤토리 정보의 일관성을 보장합니다.

# ServerLocationComponent

### `UServerLocationComponent()`
이 생성자는 `UServerLocationComponent` 클래스의 초기 설정을 담당합니다. 컴포넌트가 매 프레임마다 업데이트되도록 설정하고, 위치 데이터 유지 시간과 기록 간격을 초기화합니다. 또한 여러 캡슐 컴포넌트를 생성하여 몸통과 다리들의 위치 및 적중 판단을 위한 컴포넌트로 초기화합니다.

### `BeginPlay()`
게임 시작 시 호출되어, 컴포넌트에 필요한 초기화 작업을 수행합니다.

### `TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)`
프레임마다 호출되며, 서버에서 실행될 때만 위치 기록 및 오래된 데이터 삭제를 수행합니다. 위치 기록 간격을 확인하고 지정된 간격마다 위치를 기록합니다.

### `RecordLocation()`
캐릭터의 현재 위치와 회전을 기록합니다. 이 메서드는 몸통과 다리들의 위치를 구하고 이를 기록하여 위치 데이터를 유지합니다. 위치 기록은 각 컴포넌트의 현재 위치와 회전을 포착하여 저장합니다.

### `CleanupOldData()`
지정된 시간 이상된 데이터를 삭제합니다. 이 메서드는 위치 기록을 관리하여 오래된 데이터가 저장 공간을 차지하지 않도록 합니다.

### `GetInterpolatedLocationData(float Time)`
주어진 시간에 해당하는 위치 데이터를 보간하여 반환합니다. 이 메서드는 두 기록된 데이터 포인트 사이에서 선형 보간을 수행하여, 요청된 시간에 가장 근접한 위치와 회전 데이터를 제공합니다.

### `CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)`
지정된 트레이스 시작과 끝 위치를 사용하여 캐릭터의 몸통과 다리에 대한 충돌 검사를 수행합니다. 이 메서드는 캐릭터의 몸통과 다리에 대한 트레이스 충돌 검사를 수행하고 결과를 반환합니다.

### `ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)`
서버에서 실행되며, 캐릭터의 몸통과 다리에 대한 트레이스를 수행합니다. 이 메서드는 서버에서 캐릭터의 위치 데이터를 사용하여 트레이스 검사를 수행하고, 충돌 결과를 반환합니다.

### `GetLocationData()`
캐릭터의 현재 위치와 회전 데이터를 반환합니다. 이 메서드는 캐릭터의 몸통과 다리의 현재 위치와 회전을 포착하여 반환합니다.

### `UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData)`
지정된 캡슐 컴포넌트의 위치와 회전을 업데이트합니다. 이 메서드는 트레이스 검사 시 사용되는 캡슐 컴포넌트의 위치와 회전을 지정된 데이터로 설정합니다.

### `PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule)`
지정된 시작점과 끝점을 사용하여 선형 트레이스를 수행하고, 주어진 캡슐 컴포넌트와 충돌이 발생했는지 검사합니다. 이 메서드는 트레이스와 캡슐 컴포넌트 간의 충돌 검사를 수행합니다.

### `RestoreCollisionSettings()`
모든 캡슐 컴포넌트의 충돌설정을 복원합니다. 이 메서드는 트레이스 수행 후 캡슐 컴포넌트의 충돌 설정을 초기 상태로 복원합니다.
