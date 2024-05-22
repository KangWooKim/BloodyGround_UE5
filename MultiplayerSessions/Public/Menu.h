// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Menu.generated.h"

/**
 * UMenu Ŭ������ ��Ƽ�÷��̾� ���� �޴��� �����ϴ� UUserWidget�� ����Ŭ�����Դϴ�.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * �޴��� �����ϴ� �Լ��Դϴ�.
     * @param NumberOfPublicConnections ���� ������ �ִ� ���� ��
     * @param TypeOfMatch ��ġ�� Ÿ��
     * @param LobbyPath �κ� ���� ���
     */
    UFUNCTION(BlueprintCallable)
        void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")));

protected:
    /**
     * ������ �ʱ�ȭ�ϴ� �Լ��Դϴ�.
     * @return �ʱ�ȭ ���� ���θ� ��ȯ�մϴ�.
     */
    virtual bool Initialize() override;

    /**
     * ������ ������ �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    virtual void NativeConstruct() override;

    /**
     * ������ �ı��� �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    virtual void NativeDestruct() override;

    /**
     * ���� ���� ����� ó���ϴ� �Լ��Դϴ�.
     * @param bWasSuccessful ���� ���� ���� ����
     */
    UFUNCTION()
        void OnCreateSession(bool bWasSuccessful);

    /**
     * ���� �˻� ����� ó���ϴ� �Լ��Դϴ�.
     * @param SessionResults ���� �˻� ��� ����Ʈ
     * @param bWasSuccessful ���� �˻� ���� ����
     */
    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

    /**
     * ���� ���� ����� ó���ϴ� �Լ��Դϴ�.
     * @param Result ���� ���� ��� Ÿ��
     */
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

    /**
     * ���� ���� ����� ó���ϴ� �Լ��Դϴ�.
     * @param bWasSuccessful ���� ���� ���� ����
     */
    UFUNCTION()
        void OnDestroySession(bool bWasSuccessful);

    /**
     * ���� ���� ����� ó���ϴ� �Լ��Դϴ�.
     * @param bWasSuccessful ���� ���� ���� ����
     */
    UFUNCTION()
        void OnStartSession(bool bWasSuccessful);

private:
    // Host ��ư UI ���
    UPROPERTY(meta = (BindWidget))
        class UButton* HostButton;

    // �� �̸� �Է¶� UI ���
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* RoomNameTextBox;

    // �� ��й�ȣ �Է¶� UI ���
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* RoomPasswordTextBox;

    // ù ��° �� ���� üũ�ڽ� UI ���
    UPROPERTY(meta = (BindWidget))
        UCheckBox* MapOption1;

    // �� ��° �� ���� üũ�ڽ� UI ���
    UPROPERTY(meta = (BindWidget))
        UCheckBox* MapOption2;

    // Create ��ư UI ���
    UPROPERTY(meta = (BindWidget))
        UButton* CreateButton;

    // ������ �� �̸� �Է¶� UI ���
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* JoinRoomNameTextBox;

    // ������ �� ��й�ȣ �Է¶� UI ���
    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* JoinRoomPasswordTextBox;

    // Join ��ư UI ���
    UPROPERTY(meta = (BindWidget))
        UButton* JoinButton;

    // Play ��ư UI ���
    UPROPERTY(meta = (BindWidget))
        UButton* PlayButton;

    /**
     * Join ��ư Ŭ�� �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    UFUNCTION()
        void OnJoinButtonClicked();

    /**
     * Play ��ư Ŭ�� �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    UFUNCTION()
        void OnPlayButtonClicked();

    /**
     * Create ��ư Ŭ�� �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    UFUNCTION()
        void OnCreateButtonClicked();

    /**
     * Host ��ư Ŭ�� �� ȣ��Ǵ� �Լ��Դϴ�.
     */
    UFUNCTION()
        void OnHostButtonClicked();

    /**
     * �� ���� �ɼ� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
     * @param bIsChecked üũ ����
     */
    UFUNCTION()
        void OnMapOptionChanged(bool bIsChecked);

    /**
     * Host �޴��� ���ü��� �����ϴ� �Լ��Դϴ�.
     * @param bIsVisible ���ü� ����
     */
    void SetHostMenuVisibility(bool bIsVisible);

    /**
     * Join �޴��� ���ü��� �����ϴ� �Լ��Դϴ�.
     * @param bIsVisible ���ü� ����
     */
    void SetJoinMenuVisibility(bool bIsVisible);

    /**
     * �޴��� �ݰ� �Է� ��带 ���� �������� ��ȯ�ϴ� �Լ��Դϴ�.
     */
    void MenuTearDown();

    // ��Ƽ�÷��̾� ���� ����� ó���ϴ� ����ý���
    class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

    // ���� ������ �ִ� ���� ��
    int32 NumPublicConnections{ 4 };

    // ��ġ�� Ÿ��
    FString MatchType{ TEXT("FreeForAll") };

    // �κ� ���� ���
    FString PathToLobby{ TEXT("") };

    // ���õ� ��
    FString SelectedMap;
};
