// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * UMultiplayerSessionsSubsystem Ŭ������ ��Ƽ�÷��̾� ���� ����� ó���ϴ� ����ý����Դϴ�.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	/**
	 * ������ �����մϴ�.
	 * @param NumPublicConnections ���� ������ ��
	 * @param MatchType ��ġ Ÿ��
	 * @param RoomName �� �̸�
	 * @param RoomPassword �� ��й�ȣ
	 */
	void CreateSession(int32 NumPublicConnections, FString MatchType, FString RoomName, FString RoomPassword);

	/**
	 * ���ǿ� �����մϴ�.
	 * @param SessionResult ���� �˻� ���
	 */
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	/**
	 * ������ �ı��մϴ�.
	 */
	void DestroySession();

	/**
	 * ������ �����մϴ�.
	 */
	void StartSession();

	/**
	 * ������ ã���ϴ�.
	 * @param RoomName �� �̸�
	 * @param RoomPassword �� ��й�ȣ
	 */
	void FindSessions(FString RoomName, FString RoomPassword);

	//
	// Our own custom delegates for the Menu class to bind callbacks to
	//
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

protected:

	
	/**
	 * ���� ���� �Ϸ� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
	 * @param SessionName ���� �̸�
	 * @param bWasSuccessful ���� ���� ���� ����
	 */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * ���� �˻� �Ϸ� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
	 * @param bWasSuccessful ���� �˻� ���� ����
	 */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	 * ���� ���� �Ϸ� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
	 * @param SessionName ���� �̸�
	 * @param Result ���� ���� ��� Ÿ��
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * ���� �ı� �Ϸ� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
	 * @param SessionName ���� �̸�
	 * @param bWasSuccessful ���� �ı� ���� ����
	 */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * ���� ���� �Ϸ� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
	 * @param SessionName ���� �̸�
	 * @param bWasSuccessful ���� ���� ���� ����
	 */
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FString LastRoomName;
	FString LastRoomPassword;

	// ���� �˻� ����� ó���ϴ� ��������Ʈ �Լ�
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{ false };
	int32 LastNumPublicConnections;
	FString LastMatchType;
	FString SelectedMapName;
};
