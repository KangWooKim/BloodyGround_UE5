# BloodyGroundPlayerController

### `ABloodyGroundPlayerController()`
이 생성자는 `ABloodyGroundPlayerController` 클래스의 초기 설정을 담당합니다. 여기서는 플레이어 컨트롤러의 RTT(Round-Trip Time) 측정에 필요한 변수를 초기화합니다. RTT는 네트워크 지연을 측정하는 데 사용되며, 게임 내에서의 플레이어 경험에 중요한 영향을 미칩니다.

### `PlayerTick(float DeltaTime)`
매 프레임마다 호출되는 이 메서드는 플레이어 컨트롤러의 주요 틱 함수입니다. 여기서는 RTT를 주기적으로 업데이트하여 네트워크 지연을 측정합니다. 이 메서드는 지난 RTT 업데이트 시간 이후로 1초가 경과했는지 확인하고, 조건이 만족되면 로컬 컨트롤러의 플레이어 상태를 사용하여 새로운 RTT를 계산합니다. 이 계산은 플레이어의 핑 값을 초 단위로 변환하여 저장함으로써 이루어집니다. 이 정보는 네트워크 상태의 질을 플레이어에게 피드백하는 데 사용될 수 있습니다.
