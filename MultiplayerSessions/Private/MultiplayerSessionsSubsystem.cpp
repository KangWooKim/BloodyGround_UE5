// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// UMultiplayerSessionsSubsystem Ŭ������ �������Դϴ�.
// �پ��� ���� ���� ��������Ʈ�� �ʱ�ȭ�ϰ� Online Subsystem�� �����մϴ�.
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// Online Subsystem�� �����ɴϴ�.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// ���� �������̽��� �����ɴϴ�.
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

// ������ �����ϴ� �Լ��Դϴ�.
// @param NumPublicConnections ���� ������ ��
// @param MatchType ��ġ Ÿ��
// @param RoomName �� �̸�
// @param RoomPassword �� ��й�ȣ
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType, FString RoomName, FString RoomPassword)
{
	// ���� �������̽��� ��ȿ���� Ȯ���մϴ�.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// ���� ������ �ִ��� Ȯ���մϴ�.
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		// ���� ������ �ı��մϴ�.
		DestroySession();
	}

	// ��������Ʈ�� �߰��մϴ�.
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// ���� ������ �ʱ�ȭ�մϴ�.
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
	// ������ �����մϴ�.
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// ��������Ʈ�� �����մϴ�.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// ���� ���� ���и� �˸��ϴ�.
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

// ���ǿ� �����ϴ� �Լ��Դϴ�.
// @param SessionResult ���� �˻� ���
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// ���ǿ� �����մϴ�.
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

// ������ ã�� �Լ��Դϴ�.
// @param RoomName �� �̸�
// @param RoomPassword �� ��й�ȣ
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

			// �� �̸��� ��й�ȣ ���� �߰�
			SessionSearch->QuerySettings.Set(FName("RoomName"), RoomName, EOnlineComparisonOp::Equals);
			SessionSearch->QuerySettings.Set(FName("RoomPassword"), RoomPassword, EOnlineComparisonOp::Equals);

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
				FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete)
			);

			// ������ ã���ϴ�.
			Sessions->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
		}
	}
}

// ������ �ı��ϴ� �Լ��Դϴ�.
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// ������ �ı��մϴ�.
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

// ������ �����ϴ� �Լ��Դϴ�.
void UMultiplayerSessionsSubsystem::StartSession()
{
	// ���� ������ ����ֽ��ϴ�.
}

// ���� ������ �Ϸ�Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
// @param SessionName ���� �̸�
// @param bWasSuccessful ���� ���� ���� ����
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// ���� ���� �ϷḦ ��ε�ĳ��Ʈ�մϴ�.
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

// ���� �˻��� �Ϸ�Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
// @param bWasSuccessful ���� �˻� ���� ����
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// �˻� ����� ���ų� ������ ���� ���
	if (!SessionSearch.IsValid() || SessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// ���� �˻� �ϷḦ ��ε�ĳ��Ʈ�մϴ�.
	MultiplayerOnFindSessionsComplete.Broadcast(SessionSearch->SearchResults, bWasSuccessful);
}

// ���� ������ �Ϸ�Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
// @param SessionName ���� �̸�
// @param Result ���� ���� ��� Ÿ��
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

	// ���� ���� �ϷḦ ��ε�ĳ��Ʈ�մϴ�.
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

// ���� �ı��� �Ϸ�Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
// @param SessionName ���� �̸�
// @param bWasSuccessful ���� �ı� ���� ����
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
	// ���� �ı� �ϷḦ ��ε�ĳ��Ʈ�մϴ�.
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

// ���� ������ �Ϸ�Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
// @param SessionName ���� �̸�
// @param bWasSuccessful ���� ���� ���� ����
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// ���� ������ ����ֽ��ϴ�.
}
