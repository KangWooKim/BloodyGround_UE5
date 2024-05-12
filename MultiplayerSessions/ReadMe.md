# Menu

### `UMenu::NativeConstruct()`
이 메서드는 메뉴 위젯이 생성될 때 호출되며, UI 컴포넌트의 초기 설정과 이벤트 바인딩을 담당합니다. 각 버튼(생성, 호스트, 참가, 플레이)과 맵 옵션에 대한 클릭 또는 체크 상태 변경 이벤트를 바인딩합니다. 추가적으로, 참가 및 호스트 메뉴의 초기 가시성을 설정하여 사용자 인터페이스의 시작 상태를 정의합니다.

### `UMenu::OnMapOptionChanged(bool bIsChecked)`
맵 옵션 선택 상태가 변경될 때 호출되며, 한 맵 옵션이 선택되면 다른 맵 옵션의 선택을 해제합니다. 이 로직은 사용자가 한 번에 하나의 맵만 선택할 수 있도록 강제합니다.

### `UMenu::OnJoinButtonClicked()`
"Join" 버튼이 클릭되었을 때 호출되어, 참가 메뉴의 가시성을 '보이게' 설정합니다. 이는 사용자가 게임에 참가하기 위해 필요한 옵션들을 입력할 수 있게 합니다.

### `UMenu::OnPlayButtonClicked()`
"Play" 버튼이 클릭되었을 때 호출되며, 입력된 방 이름과 비밀번호를 사용하여 세션을 찾는 로직을 실행합니다. 이는 멀티플레이 세션 시스템과 연동되어 작동합니다.

### `UMenu::SetJoinMenuVisibility(bool bIsVisible)`
참가 메뉴의 가시성을 설정하는 메서드로, 참가 관련 UI 요소들의 가시성을 조절합니다.

### `UMenu::OnCreateButtonClicked()`
"Create" 버튼이 클릭되었을 때 호출되어, 입력된 방 이름과 비밀번호, 선택된 맵을 사용하여 새 게임 세션을 생성합니다. 이 메서드는 멀티플레이 세션 생성 로직을 포함합니다.

### `UMenu::OnHostButtonClicked()`
"Host" 버튼이 클릭되었을 때 호출되어, 호스트 메뉴의 가시성을 '보이게' 설정합니다. 이는 사용자가 게임 호스팅을 위해 필요한 옵션들을 입력할 수 있게 합니다.

### `UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)`
메뉴를 설정하는 메서드로, 공개 연결 수, 매치 타입, 로비 경로를 설정합니다. 이 정보는 멀티플레이 세션의 구성에 사용됩니다.

### `UMenu::Initialize()`
위젯 초기화 시 호출되며, 부모 클래스의 `Initialize` 메서드를 호출합니다. 초기화가 성공적으로 완료되면 `true`를 반환합니다.

### `UMenu::MenuTearDown()`
메뉴 위젯이 제거될 때 호출되어, UI를 정리하고 게임 모드로 입력 모드를 전환합니다. 이는 사용자가 메뉴를 벗어날 때 게임 조작을 바로 시작할 수 있도록 설정합니다.

### `UMenu::NativeDestruct()`
위젯이 파괴될 때 호출되어, `MenuTearDown`을 실행하고 부모 클래스의 `NativeDestruct`를 호출합니다. 이 메서드는 위젯의 정리 작업을 보장합니다.

# MultyPlayerSessions

### `StartupModule()`
이 메서드는 `FMultiplayerSessionsModule` 모듈이 메모리에 로드될 때 호출됩니다. 정확한 시점은 모듈별로 `.uplugin` 파일에 명시되어 있습니다. 이 메서드는 모듈 시작 시 필요한 리소스 초기화나 상태 설정을 위해 사용됩니다. 개발자는 이곳에 필수적인 운영을 위한 훅 등록, 외부 라이브러리 초기화, 연결 설정 등의 코드를 추가할 수 있습니다.

### `ShutdownModule()`
이 메서드는 모듈이 종료될 때 호출될 수 있습니다. 동적으로 리로드가 지원되는 모듈의 경우, 모듈을 언로드하기 전에 이 함수를 호출합니다. 모듈을 정리하는 데 필요한 작업을 수행합니다.

# MultiPlayerSessionsSybsystem

### `UMultiplayerSessionsSubsystem()`
이 생성자는 멀티플레이어 세션 시스템의 초기 설정을 담당합니다. 온라인 서브시스템에서 세션 인터페이스를 가져오고, 각 세션 이벤트에 대한 콜백을 위임(delegate)으로 연결합니다.

### `CreateSession(int32 NumPublicConnections, FString MatchType, FString RoomName, FString RoomPassword)`
새 게임 세션을 생성하는 기능을 수행합니다. 이 메서드는 공개 연결 수, 매치 타입, 방 이름, 방 비밀번호를 파라미터로 받아 세션을 생성합니다. 세션 설정에 필요한 모든 정보를 설정 객체에 저장하고, 세션 생성 요청을 온라인 서브시스템에 전달합니다.

### `JoinSession(const FOnlineSessionSearchResult& SessionResult)`
특정 세션 결과를 사용하여 세션에 참여합니다. 이 메서드는 찾아진 세션 중 하나에 참여하고자 할 때 호출됩니다. 사용자의 고유 네트워크 ID와 함께 참여할 세션의 정보를 온라인 서브시스템에 전달합니다.

### `DestroySession()`
현재 활성화된 세션을 종료하고 제거합니다. 세션을 제거할 수 있는 권한이 있는지 확인한 후, 세션 제거 요청을 온라인 서브시스템에 전달합니다.

### `StartSession()`
활성화된 세션을 시작합니다. 이 메서드는 세션 시작 프로세스를 초기화할 때 사용됩니다.

### `FindSessions(FString RoomName, FString RoomPassword)`
사용 가능한 세션을 검색하는 기능을 수행합니다. 방 이름과 비밀번호를 사용하여 특정 조건에 맞는 세션을 검색하고, 결과를 처리하기 위한 콜백을 설정합니다.

### `OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)`
세션 생성이 완료되었을 때 호출되는 콜백 함수입니다. 성공적으로 세션을 생성했는지 여부에 따라 다음 단계를 진행하거나, 실패 시 오류 메시지를 처리합니다.

### `OnFindSessionsComplete(bool bWasSuccessful)`
세션 검색이 완료되었을 때 호출되는 콜백 함수입니다. 검색 결과를 분석하고, 성공적으로 세션을 찾았는지 여부를 다룹니다.

### `OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)`
세션 참여 요청이 완료되었을 때 호출되는 콜백 함수입니다. 참여 성공 여부를 확인하고, 실패 시 다음 단계를 결정합니다.

### `OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)`
세션 제거가 완료되었을 때 호출되는 콜백 함수입니다. 세션 제거의 성공 여부를 확인하고, 필요한 경우 새 세션을 생성합니다.

### `OnStartSessionComplete(FName SessionName, bool bWasSuccessful)`
세션 시작이 완료되었을 때 호출되는 콜백 함수입니다. 세션 시작의 성공 여부를 확인합니다.
