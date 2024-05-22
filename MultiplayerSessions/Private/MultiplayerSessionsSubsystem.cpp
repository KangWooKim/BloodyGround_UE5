// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// UMultiplayerSessionsSubsystem 클래스의 생성자입니다.
// 다양한 세션 관련 델리게이트를 초기화하고 Online Subsystem을 설정합니다.
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// Online Subsystem을 가져옵니다.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// 세션 인터페이스를 가져옵니다.
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

// 세션을 생성하는 함수입니다.
// @param NumPublicConnections 공개 연결의 수
// @param MatchType 매치 타입
// @param RoomName 방 이름
// @param RoomPassword 방 비밀번호
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType, FString RoomName, FString RoomPassword)
{
	// 세션 인터페이스가 유효한지 확인합니다.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// 기존 세션이 있는지 확인합니다.
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		// 기존 세션을 파괴합니다.
		DestroySession();
	}

	// 델리게이트를 추가합니다.
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// 세션 설정을 초기화합니다.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("CurrentMap"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("RoomName"), RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->Set(FName("RoomPassword"), RoomPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastRoomName = RoomName;
	LastRoomPassword = RoomPassword;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// 세션을 생성합니다.
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// 델리게이트를 제거합니다.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// 세션 생성 실패를 알립니다.
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

// 세션에 참가하는 함수입니다.
// @param SessionResult 세션 검색 결과
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// 세션에 참가합니다.
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

// 세션을 찾는 함수입니다.
// @param RoomName 방 이름
// @param RoomPassword 방 비밀번호
void UMultiplayerSessionsSubsystem::FindSessions(FString RoomName, FString RoomPassword)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		if (Sessions.IsValid() && !SessionSearch.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 5;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			// 방 이름과 비밀번호 필터 추가
			SessionSearch->QuerySettings.Set(FName("RoomName"), RoomName, EOnlineComparisonOp::Equals);
			SessionSearch->QuerySettings.Set(FName("RoomPassword"), RoomPassword, EOnlineComparisonOp::Equals);

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete)
			);

			// 세션을 찾습니다.
			Sessions->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
		}
	}
}

// 세션을 파괴하는 함수입니다.
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// 세션을 파괴합니다.
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

// 세션을 시작하는 함수입니다.
void UMultiplayerSessionsSubsystem::StartSession()
{
	// 현재 구현은 비어있습니다.
}

// 세션 생성이 완료되었을 때 호출되는 콜백 함수입니다.
// @param SessionName 세션 이름
// @param bWasSuccessful 세션 생성 성공 여부
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 세션 생성 완료를 브로드캐스트합니다.
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

// 세션 검색이 완료되었을 때 호출되는 콜백 함수입니다.
// @param bWasSuccessful 세션 검색 성공 여부
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// 검색 결과가 없거나 세션이 없을 경우
	if (!SessionSearch.IsValid() || SessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// 세션 검색 완료를 브로드캐스트합니다.
	MultiplayerOnFindSessionsComplete.Broadcast(SessionSearch->SearchResults, bWasSuccessful);
}

// 세션 참가가 완료되었을 때 호출되는 콜백 함수입니다.
// @param SessionName 세션 이름
// @param Result 세션 참가 결과 타입
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString(TEXT("OnJoinSession called"))
		);
	}
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// 세션 참가 완료를 브로드캐스트합니다.
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

// 세션 파괴가 완료되었을 때 호출되는 콜백 함수입니다.
// @param SessionName 세션 이름
// @param bWasSuccessful 세션 파괴 성공 여부
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType, LastRoomName, LastRoomPassword);
	}
	// 세션 파괴 완료를 브로드캐스트합니다.
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

// 세션 시작이 완료되었을 때 호출되는 콜백 함수입니다.
// @param SessionName 세션 이름
// @param bWasSuccessful 세션 시작 성공 여부
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 현재 구현은 비어있습니다.
}
