// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGameInstance.h"
#include "LobbyFind.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinCancel);

class USButtonCustom;

/**
 * 
 */
UCLASS()
class COOPGAME_API ULobbyFind : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	bool Initialize() override;


public:
	ULobbyFind(const FObjectInitializer& ObjectInitializer);

	/** Function that will talk to game instance to re-populate the server list */
	void RestartLobbySearch();

	/** Function to stop any lobby search, just in case one is happening, talks to game instance */
	void StopLobbySearch();

public:
	/** Delegate for when button object is clicked */
	FOnJoinCancel OnJoinCancel;


private:
	UFUNCTION()
	void OnCancelClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnRefreshClicked();

	UFUNCTION()
	void RepopulateServerList(TArray<FServerData> ServerList);

	UFUNCTION()
	void CancelFindComplete(bool bWasSuccessful);

	/** Updates our state, if currently searching for lobbies or not */
	void ChangeToSearching(bool bIsSearching);

private:
	// Store reference to game instance to use for Session Interface calls
	class USGameInstance* GI;

	UPROPERTY(meta = (BindWidget))
	USButtonCustom* CancelButton;

	UPROPERTY(meta = (BindWidget))
	USButtonCustom* JoinButton;

	UPROPERTY(meta = (BindWidget))
	USButtonCustom* RefreshButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* FindLobbySwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* FindingLobbiesText;

	UPROPERTY(meta = (BindWidget))
	class UWidget* FindingLobbiesRefresh;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerListScrollBox;

	TSubclassOf<class UServerEntry> ServerEntryClass;
};
