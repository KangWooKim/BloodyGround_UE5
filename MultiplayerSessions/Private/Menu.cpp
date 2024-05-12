// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();

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

	if (MapOption1)
	{
		MapOption1->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);
	}

	if (MapOption2)
	{
		MapOption2->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);
	}

	SetJoinMenuVisibility(false);

	// 초기에는 관련 UI 요소들을 숨깁니다.
	SetHostMenuVisibility(false);
}

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

void UMenu::OnJoinButtonClicked()
{
	SetJoinMenuVisibility(true);
}

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

void UMenu::SetJoinMenuVisibility(bool bIsVisible)
{
	ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	JoinRoomNameTextBox->SetVisibility(NewVisibility);
	JoinRoomPasswordTextBox->SetVisibility(NewVisibility);
	PlayButton->SetVisibility(NewVisibility);
}

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

void UMenu::OnHostButtonClicked()
{
	// 'Host' 버튼 클릭 시, 관련 UI 요소들을 보이게 합니다.
	SetHostMenuVisibility(true);
}

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

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

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


void UMenu::SetHostMenuVisibility(bool bIsVisible)
{
	ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	RoomNameTextBox->SetVisibility(Visibility);
	RoomPasswordTextBox->SetVisibility(Visibility);
	MapOption1->SetVisibility(Visibility);
	MapOption2->SetVisibility(Visibility);
	CreateButton->SetVisibility(Visibility);
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}


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

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}