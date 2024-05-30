# BloodyGroundGameState

### ABloodyGroundGameState()
클래스의 생성자로, 게임 내에서 낮과 밤의 주기를 관리하기 위한 초기 설정을 담당합니다. 광원(Sunlight) 객체를 초기화합니다.

### BeginPlay()
게임이 시작될 때 호출됩니다. 서버 권한이 있는 경우, 낮/밤 주기를 주기적으로 변경하는 타이머를 설정합니다. 예를 들어, 60초마다 `ToggleDayNightCycle` 함수가 호출되어 낮과 밤이 전환됩니다.

### SetDayNightCycle(EDayNightCycle NewCycle)
낮/밤 상태를 설정하는 함수입니다. 서버에서 호출되어 상태를 변경하고, 클라이언트에서는 `OnRep_DayNightChanged` 함수를 통해 상태 변경을 반영합니다.

### OnRep_DayNightChanged()
낮/밤 상태가 변경될 때 호출되며, 현재 상태에 따라 햇빛을 조정합니다. `AdjustSunlight` 함수를 호출하여 햇빛의 강도를 변경합니다.

### GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override
네트워크 복제를 위한 속성을 설정합니다. `DayNightCycle` 변수를 네트워크에서 복제할 수 있도록 설정하여, 클라이언트가 낮/밤 상태를 동기화할 수 있게 합니다.

### AdjustSunlight(bool bIsDay)
낮 또는 밤에 따라 햇빛의 강도를 조정합니다. 목표 광원 세기를 설정하고, 서서히 변경하기 위한 타이머를 설정합니다. 낮이면 밝게, 밤이면 어둡게 설정합니다.

### UpdateSunlight()
햇빛의 강도를 서서히 목표 값으로 변경합니다. 현재 광원 세기와 목표 광원 세기의 차이를 줄여 나가며, 목표 값에 도달하면 타이머를 중지합니다.

### ToggleDayNightCycle()
현재 낮/밤 상태를 반대로 변경합니다. 낮 상태에서 호출되면 밤 상태로 변경하고, 밤 상태에서 호출되면 낮 상태로 변경합니다.
