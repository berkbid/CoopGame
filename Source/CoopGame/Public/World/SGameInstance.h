// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SGameInstance.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString ServerName;
	FString HostUsername;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;

	FServerData()
	{
		ServerName = "DefaultServerName";
		HostUsername = "DefaultHostName";
		CurrentPlayers = 0;
		MaxPlayers = 0;
	}

	FServerData(const FString& NewServerName, const FString& NewHostName, uint16 NewCurrentPlayers, uint16 NewMaxPlayers)
	{
		ServerName = NewServerName;
		HostUsername = NewHostName;
		CurrentPlayers = NewCurrentPlayers;
		MaxPlayers = NewMaxPlayers;
	}

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServersRefreshed, TArray<FServerData>, ServerList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCancelFindComplete, bool, bWasSuccessful);


/**
 * 
 */
UCLASS()
class COOPGAME_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	void Init() override;

	// Takes arguments and calls CreateSession() on the Online Subsystem SessionInterface shared pointer
	void CreateSession(const FString& LobbyName, int32 NumConnections = 4, bool bAdvertise = true);

	void FindSessions();

	void CancelFindSessions();

public:
	// Event for blueprint to hook on when actor gets killed
	FOnServersRefreshed OnServersRefreshed;

	// Event for blueprint to hook on when actor gets killed
	FOnCancelFindComplete OnCancelFindComplete;

protected:
	void HandleCreateSessionComplete(FName SessionName, bool bDidCreate);

	void HandleDestroySessionComplete(FName SessionName, bool bDidDestroy);

	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionResult);

	void HandleRegisterPlayersComplete(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& PlayerIDs, bool bWasSuccessful);

	void HandleUnRegisterPlayersComplete(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& PlayerIDs, bool bWasSuccessful);

	void HandleCancelFindSessionsComplete(bool bWasSuccessful);

	/**
	 * Delegate fired when the search for an online session has completed
	 *
	 * @param bWasSuccessful true if the async action completed without error, false if there was an error
	 */
	void HandleFindSessionsComplete(bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	// Shared pointer to store session search
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};
