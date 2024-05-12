# KeyItem

### `AKeyItem()`
이 생성자는 `AKeyItem` 클래스의 초기 설정을 담당합니다. 게임 내에서 키 아이템의 표현을 위한 메시 컴포넌트와 네트워크 복제 설정, 충돌 감지를 위한 구체 컴포넌트를 추가합니다. 충돌 구성 요소는 키 아이템을 캐릭터가 수집할 수 있도록 설정하며, 이벤트 바인딩을 통해 플레이어와의 충돌을 처리합니다.

### `BeginPlay()`
게임 플레이가 시작될 때 호출되며, 아이템의 시작 위치를 기록합니다. 이 위치 정보는 아이템의 애니메이션 움직임 계산에 사용됩니다.

### `Tick(float DeltaTime)`
매 프레임마다 호출되어, 아이템의 부유 애니메이션을 처리합니다. 삼각함수를 사용하여 아이템의 Z 위치를 주기적으로 변경하여 위아래로 움직이게 합니다.

### `OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)`
플레이어나 다른 객체가 키 아이템과 충돌했을 때 호출됩니다. 충돌한 객체가 플레이어일 경우, 서버에서 아이템을 제거하고 관련 효과음을 재생하는 처리를 수행합니다.

### `Multicast_PlaySoundAndDestroy_Implementation()`
네트워크 상의 모든 클라이언트에서 키 아이템의 획득 사운드를 재생하고, 아이템을 게임 세계에서 제거합니다. 이 멀티캐스트 함수는 아이템 획득이 시각적 및 청각적으로 동기화되게 합니다.
