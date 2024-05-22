// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// UMenu 클래스의 생성자. 위젯이 생성될 때 호출됩니다.
void UMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트를 바인딩합니다.
    if (CreateButton)
    {
        CreateButton->OnClicked.AddDynamic(this, &UMenu::OnCreateButtonClicked);
    }

    if (HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &UMenu::OnHostButtonClicked);
    }

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &UMenu::OnJoinButtonClicked);
    }

    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMenu::OnPlayButtonClicked);
    }

    // 맵 선택 옵션 변경 이벤트를 바인딩합니다.
    if (MapOption1)
    {
        MapOption1->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);
    }

    if (MapOption2)
    {
        MapOption2->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);
    }

    // 초기에는 관련 UI 요소들을 숨깁니다.
    SetJoinMenuVisibility(false);
    SetHostMenuVisibility(false);
}

// 맵 옵션이 변경되었을 때 호출되는 함수
void UMenu::OnMapOptionChanged(bool bIsChecked)
{
    if (MapOption1 && MapOption2)
    {
        // MapOption1이 선택되면 MapOption2의 선택을 해제합니다.
        if (MapOption1->IsChecked() && bIsChecked)
        {
            MapOption2->SetIsChecked(false);
        }
        // MapOption2가 선택되면 MapOption1의 선택을 해제합니다.
        else if (MapOption2->IsChecked() && bIsChecked)
        {
            MapOption1->SetIsChecked(false);
        }
    }
}

// Join 버튼이 클릭되었을 때 호출되는 함수
void UMenu::OnJoinButtonClicked()
{
    SetJoinMenuVisibility(true);
}

// Play 버튼이 클릭되었을 때 호출되는 함수
void UMenu::OnPlayButtonClicked()
{
    FString RoomName = JoinRoomNameTextBox->GetText().ToString();
    FString RoomPassword = JoinRoomPasswordTextBox->GetText().ToString();

    // 세션 참가 로직을 호출
    UMultiplayerSessionsSubsystem* SessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
    if (SessionsSubsystem)
    {
        SessionsSubsystem->FindSessions(RoomName, RoomPassword);
    }
}

// Join 메뉴의 가시성을 설정하는 함수
void UMenu::SetJoinMenuVisibility(bool bIsVisible)
{
    ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

    JoinRoomNameTextBox->SetVisibility(NewVisibility);
    JoinRoomPasswordTextBox->SetVisibility(NewVisibility);
    PlayButton->SetVisibility(NewVisibility);
}

// Create 버튼이 클릭되었을 때 호출되는 함수
void UMenu::OnCreateButtonClicked()
{
    FString RoomName = RoomNameTextBox->GetText().ToString();
    FString RoomPassword = RoomPasswordTextBox->GetText().ToString();

    // 맵 선택 처리
    if (MapOption1->IsChecked())
    {
        SelectedMap = "MapOption1";
    }
    else if (MapOption2->IsChecked())
    {
        SelectedMap = "MapOption2";
    }

    UMultiplayerSessionsSubsystem* SessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();

    if (SessionsSubsystem)
    {
        SessionsSubsystem->CreateSession(4, SelectedMap, RoomName, RoomPassword);
    }
}

// Host 버튼이 클릭되었을 때 호출되는 함수
void UMenu::OnHostButtonClicked()
{
    // 'Host' 버튼 클릭 시, 관련 UI 요소들을 보이게 합니다.
    SetHostMenuVisibility(true);
}

// 메뉴를 설정하는 함수
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
    }
}

// 초기화 함수
bool UMenu::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    return true;
}

// 세션 생성 완료 시 호출되는 함수
void UMenu::OnCreateSession(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        FString MapPath;
        if (SelectedMap == "MapOption1")
        {
            MapPath = "/Game/Maps/MapOption1";
        }
        else if (SelectedMap == "MapOption2")
        {
            MapPath = "/Game/Maps/MapOption2";
        }

        UWorld* World = GetWorld();
        if (World)
        {
            World->ServerTravel(MapPath + FString("?listen"));
        }

    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Red,
                FString(TEXT("Failed to create session!"))
            );
        }
        HostButton->SetIsEnabled(true);
    }
}

// 세션 검색 완료 시 호출되는 함수
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
    if (MultiplayerSessionsSubsystem == nullptr)
    {
        return;
    }

    for (auto Result : SessionResults)
    {
        MultiplayerSessionsSubsystem->JoinSession(Result);
        return;

    }
    if (!bWasSuccessful || SessionResults.Num() == 0)
    {
        JoinButton->SetIsEnabled(true);
    }
}

// 세션 참가 완료 시 호출되는 함수
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            FString Address;
            SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
    }
}

// Host 메뉴의 가시성을 설정하는 함수
void UMenu::SetHostMenuVisibility(bool bIsVisible)
{
    ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

    RoomNameTextBox->SetVisibility(NewVisibility);
    RoomPasswordTextBox->SetVisibility(NewVisibility);
    MapOption1->SetVisibility(NewVisibility);
    MapOption2->SetVisibility(NewVisibility);
    CreateButton->SetVisibility(NewVisibility);
}

// 세션 파괴 완료 시 호출되는 함수
void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

// 세션 시작 완료 시 호출되는 함수
void UMenu::OnStartSession(bool bWasSuccessful)
{
}

// 메뉴를 닫고 UI를 정리하는 함수
void UMenu::MenuTearDown()
{
    RemoveFromParent();
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
}

// UMenu 클래스의 소멸자. 위젯이 파괴될 때 호출됩니다.
void UMenu::NativeDestruct()
{
    MenuTearDown();
    Super::NativeDestruct();
}
