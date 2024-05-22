// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Menu.generated.h"

/**
 * UMenu 클래스는 멀티플레이어 세션 메뉴를 관리하는 UUserWidget의 서브클래스입니다.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * 메뉴를 설정하는 함수입니다.
     * @param NumberOfPublicConnections 공개 세션의 최대 연결 수
     * @param TypeOfMatch 매치의 타입
     * @param LobbyPath 로비 맵의 경로
     */
    UFUNCTION(BlueprintCallable)
        void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")));

protected:
    /**
     * 위젯을 초기화하는 함수입니다.
     * @return 초기화 성공 여부를 반환합니다.
     */
    virtual bool Initialize() override;

    /**
     * 위젯이 생성될 때 호출되는 함수입니다.
     */
    virtual void NativeConstruct() override;

    /**
     * 위젯이 파괴될 때 호출되는 함수입니다.
     */
    virtual void NativeDestruct() override;

    /**
     * 세션 생성 결과를 처리하는 함수입니다.
     * @param bWasSuccessful 세션 생성 성공 여부
     */
    UFUNCTION()
        void OnCreateSession(bool bWasSuccessful);

    /**
     * 세션 검색 결과를 처리하는 함수입니다.
     * @param SessionResults 세션 검색 결과 리스트
     * @param bWasSuccessful 세션 검색 성공 여부
     */
    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

    /**
     * 세션 참가 결과를 처리하는 함수입니다.
     * @param Result 세션 참가 결과 타입
     */
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

    /**
     * 세션 삭제 결과를 처리하는 함수입니다.
     * @param bWasSuccessful 세션 삭제 성공 여부
     */
    UFUNCTION()
        void OnDestroySession(bool bWasSuccessful);

    /**
     * 세션 시작 결과를 처리하는 함수입니다.
     * @param bWasSuccessful 세션 시작 성공 여부
     */
    UFUNCTION()
        void OnStartSession(bool bWasSuccessful);

private:
    // Host 버튼 UI 요소
    UPROPERTY(meta = (BindWidget))
        class UButton* HostButton;

    // 방 이름 입력란 UI 요소
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* RoomNameTextBox;

    // 방 비밀번호 입력란 UI 요소
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* RoomPasswordTextBox;

    // 첫 번째 맵 선택 체크박스 UI 요소
    UPROPERTY(meta = (BindWidget))
        UCheckBox* MapOption1;

    // 두 번째 맵 선택 체크박스 UI 요소
    UPROPERTY(meta = (BindWidget))
        UCheckBox* MapOption2;

    // Create 버튼 UI 요소
    UPROPERTY(meta = (BindWidget))
        UButton* CreateButton;

    // 참가할 방 이름 입력란 UI 요소
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* JoinRoomNameTextBox;

    // 참가할 방 비밀번호 입력란 UI 요소
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* JoinRoomPasswordTextBox;

    // Join 버튼 UI 요소
    UPROPERTY(meta = (BindWidget))
        UButton* JoinButton;

    // Play 버튼 UI 요소
    UPROPERTY(meta = (BindWidget))
        UButton* PlayButton;

    /**
     * Join 버튼 클릭 시 호출되는 함수입니다.
     */
    UFUNCTION()
        void OnJoinButtonClicked();

    /**
     * Play 버튼 클릭 시 호출되는 함수입니다.
     */
    UFUNCTION()
        void OnPlayButtonClicked();

    /**
     * Create 버튼 클릭 시 호출되는 함수입니다.
     */
    UFUNCTION()
        void OnCreateButtonClicked();

    /**
     * Host 버튼 클릭 시 호출되는 함수입니다.
     */
    UFUNCTION()
        void OnHostButtonClicked();

    /**
     * 맵 선택 옵션 변경 시 호출되는 함수입니다.
     * @param bIsChecked 체크 여부
     */
    UFUNCTION()
        void OnMapOptionChanged(bool bIsChecked);

    /**
     * Host 메뉴의 가시성을 설정하는 함수입니다.
     * @param bIsVisible 가시성 여부
     */
    void SetHostMenuVisibility(bool bIsVisible);

    /**
     * Join 메뉴의 가시성을 설정하는 함수입니다.
     * @param bIsVisible 가시성 여부
     */
    void SetJoinMenuVisibility(bool bIsVisible);

    /**
     * 메뉴를 닫고 입력 모드를 게임 전용으로 전환하는 함수입니다.
     */
    void MenuTearDown();

    // 멀티플레이어 세션 기능을 처리하는 서브시스템
    class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

    // 공개 세션의 최대 연결 수
    int32 NumPublicConnections{ 4 };

    // 매치의 타입
    FString MatchType{ TEXT("FreeForAll") };

    // 로비 맵의 경로
    FString PathToLobby{ TEXT("") };

    // 선택된 맵
    FString SelectedMap;
};
