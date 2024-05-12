// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")));

	
protected:

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* RoomNameTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* RoomPasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MapOption1;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* MapOption2;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateButton;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* JoinRoomNameTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* JoinRoomPasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnCreateButtonClicked();

	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnMapOptionChanged(bool bIsChecked);
	

	// UI 요소의 가시성을 설정하는 함수
	void SetHostMenuVisibility(bool bIsVisible);
	void SetJoinMenuVisibility(bool bIsVisible);


	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};
	FString SelectedMap;
};
