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
 * UMultiplayerSessionsSubsystem 클래스는 멀티플레이어 세션 기능을 처리하는 서브시스템입니다.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	/**
	 * 세션을 생성합니다.
	 * @param NumPublicConnections 공개 연결의 수
	 * @param MatchType 매치 타입
	 * @param RoomName 방 이름
	 * @param RoomPassword 방 비밀번호
	 */
	void CreateSession(int32 NumPublicConnections, FString MatchType, FString RoomName, FString RoomPassword);

	/**
	 * 세션에 참가합니다.
	 * @param SessionResult 세션 검색 결과
	 */
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	/**
	 * 세션을 파괴합니다.
	 */
	void DestroySession();

	/**
	 * 세션을 시작합니다.
	 */
	void StartSession();

	/**
	 * 세션을 찾습니다.
	 * @param RoomName 방 이름
	 * @param RoomPassword 방 비밀번호
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
	 * 세션 생성 완료 시 호출되는 콜백 함수입니다.
	 * @param SessionName 세션 이름
	 * @param bWasSuccessful 세션 생성 성공 여부
	 */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * 세션 검색 완료 시 호출되는 콜백 함수입니다.
	 * @param bWasSuccessful 세션 검색 성공 여부
	 */
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	 * 세션 참가 완료 시 호출되는 콜백 함수입니다.
	 * @param SessionName 세션 이름
	 * @param Result 세션 참가 결과 타입
	 */
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	 * 세션 파괴 완료 시 호출되는 콜백 함수입니다.
	 * @param SessionName 세션 이름
	 * @param bWasSuccessful 세션 파괴 성공 여부
	 */
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	 * 세션 시작 완료 시 호출되는 콜백 함수입니다.
	 * @param SessionName 세션 이름
	 * @param bWasSuccessful 세션 시작 성공 여부
	 */
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FString LastRoomName;
	FString LastRoomPassword;

	// 세션 검색 결과를 처리하는 델리게이트 함수
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
