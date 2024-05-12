# InGameHUD

### `UpdateHealth(float HealthPercentage)`
이 메서드는 플레이어의 현재 건강 상태를 퍼센트로 받아 HUD의 건강 바를 업데이트합니다. 인게임 위젯에 표시된 건강 바의 시각적 표현을 해당 퍼센트에 맞게 조정합니다.

### `UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)`
플레이어의 현재 탄약 상태를 업데이트하는 메서드입니다. 탄창 내 탄약과 총 탄약 수를 받아 HUD에 표시합니다.

### `SetRespawnText()`
플레이어가 사망하거나 리스폰을 기다릴 때 호출되어 HUD에 리스폰 관련 텍스트를 표시합니다. 이는 플레이어가 리스폰까지 남은 시간이나 상태를 알 수 있게 해줍니다.

### `DeleteRespawnText()`
리스폰 텍스트를 HUD에서 제거하는 메서드입니다. 플레이어가 게임에 다시 참여할 준비가 되었을 때 호출되어 화면에서 리스폰 텍스트를 지웁니다.

### `BeginPlay()`
게임 플레이가 시작될 때 호출되며, HUD 위젯을 생성하고 화면에 추가합니다. 플레이어 컨트롤러를 기반으로 `InGameWidget` 인스턴스를 생성하고, 초기 탄약 정보를 업데이트하여 화면에 표시합니다.

# InGameWidget

### `UpdateHealthBar(float HealthPercentage)`
이 메서드는 HUD에 표시되는 건강 바의 비율을 업데이트합니다. 플레이어의 현재 건강 상태를 퍼센트로 받아 해당 비율로 건강 바의 표시를 조정합니다. 이는 플레이어에게 자신의 상태를 직관적으로 인지할 수 있게 해줍니다.

### `UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)`
플레이어의 탄약 상태를 HUD에 업데이트하는 메서드입니다. 탄창 내 탄약 수와 전체 보유 탄약 수를 문자열 형태로 변환하여 화면에 표시합니다.

### `SetRespawnText()`
플레이어가 사망했을 때 리스폰을 기다리는 동안 표시될 텍스트를 설정합니다. 이 메시지는 플레이어에게 현재 상태를 알려주며, 게임에서 재진입을 기다리는 동안의 정보를 제공합니다.

### `DeleteRespawnText()`
리스폰 텍스트를 화면에서 제거하는 메서드입니다. 리스폰 과정이 완료되거나 플레이어가 게임에 재진입했을 때 호출되어, HUD에서 리스폰 관련 메시지를 클리어합니다.