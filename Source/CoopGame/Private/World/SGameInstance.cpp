// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameInstance.h"
#include "Engine/Engine.h"
#include "OnlineSessionSettings.h"

const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

USGameInstance::USGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void USGameInstance::Init()
{
	// Init called after construct, every time game starts playing
	Super::Init();

	// Get reference to the online subsystem, will not find STEAM subsystem with play in editor, must launch from terminal
	static IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Online Subsystem: %s"), *Subsystem->GetSubsystemName().ToString());

		// This is an alias for a type, shared pointer means it is being reference counted and won't be deleted
		SessionInterface = Subsystem->GetSessionInterface();
		// SessionInterface.IsValid() ??
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session Interface"));
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USGameInstance::HandleCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USGameInstance::HandleDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USGameInstance::HandleFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USGameInstance::HandleJoinSessionComplete);
			SessionInterface->OnRegisterPlayersCompleteDelegates.AddUObject(this, &USGameInstance::HandleRegisterPlayersComplete);
			SessionInterface->OnUnregisterPlayersCompleteDelegates.AddUObject(this, &USGameInstance::HandleUnRegisterPlayersComplete);
			SessionInterface->OnCancelFindSessionsCompleteDelegates.AddUObject(this, &USGameInstance::HandleCancelFindSessionsComplete);

			// Want to convert object pointer to shared pointer, this line does that, also want to create this object on the heap
			// This creates the object on the heap, and stores a shared pointer to it as a class variable, if reference count = 0, then object is deleted?
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no Subsystem"));
	}

}

void USGameInstance::CreateSession(const FString& LobbyName, int32 NumConnections, bool bAdvertise)
{
	if (!SessionInterface) { return; }

	// Possible that session exists already with name GameSession
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Session with same session name GameSession"));
	}

	// HAVE TO SET THESE VALUES ON SESSION SETTINGS
	FOnlineSessionSettings SessionSettings;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting bIsLANMatch to true for SessionSettings"));
		SessionSettings.bIsLANMatch = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting bIsLANMatch to false for SessionSettings"));
		SessionSettings.bIsLANMatch = false;
	}

	// Allow 4 connections only
	SessionSettings.NumPublicConnections = NumConnections;

	// Makes this visible for FindSessions call, IMPORTANT
	SessionSettings.bShouldAdvertise = bAdvertise;

	// NEED TO USE PRESENCE TO ENABLE LOBBIES ON SERVER, this creates lobby session, look at CreateSession() !!
	SessionSettings.bUsesPresence = true;

	//test settings, needed or only works when hosted from PIE?
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsDedicated = false;

	// We are setting a key/value pair here, making a new one to store server name
	SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, LobbyName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	UE_LOG(LogTemp, Warning, TEXT("Creating Session with name: %s"), *LobbyName);

	// Session is in progress of being created, we will get a delegate response
	SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void USGameInstance::HandleCreateSessionComplete(FName SessionName, bool bDidCreate)
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session Complete: %s"), *SessionName.ToString());

	if (bDidCreate)
	{
		UWorld* World = GetWorld();
		if (!World) { return; }
		UE_LOG(LogTemp, Warning, TEXT("Travelling to Lobby"));
		// Do a server travel to the map, and open a listening connection for others to connect
		World->ServerTravel("/Game/Maps/Lobby?listen");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not create session"));
	}
}

void USGameInstance::HandleDestroySessionComplete(FName SessionName, bool bDidDestroy)
{
	if (bDidDestroy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Destroyed! %s"), *SessionName.ToString());
	}
	
	//SessionInterface->GetNamedSession(SessionName)
}

void USGameInstance::HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type SessionResult)
{
}

void USGameInstance::HandleRegisterPlayersComplete(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& PlayerIDs, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Register Players completed for Session: %s"), *SessionName.ToString());
	}
	
}

void USGameInstance::HandleUnRegisterPlayersComplete(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& PlayerIDs, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("UN-Register Players completed for Session: %s"), *SessionName.ToString());
	}
}

void USGameInstance::FindSessions()
{
	// Make sure both shared pointers are valid before continuing
	if (!SessionInterface.IsValid()) { return; }
	if (!SessionSearch.IsValid()) { return; }

	// Can specify settings for certain api like steam
	SessionSearch->bIsLanQuery = false;

	// This will make sure we search for "presence" which is lobby games!
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// We use the APPID that shares with the default game in .DefaultEngine.ini  , that's why we need many search results
	SessionSearch->MaxSearchResults = 100;

	UE_LOG(LogTemp, Warning, TEXT("Find Sessions Started"));
	// Convert shared pointer to shared ref, shared pointer MUST be valid
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

}

void USGameInstance::HandleFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find session completed Successfully"));

		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		if (SearchResults.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Find Complete with 0 results"));
		}

		TArray<FServerData> DataArray;
		//NameArray.Add("TEST SERVER 1");
		//NameArray.Add("TEST SERVER 2");
		//NameArray.Add("TEST SERVER 3");

		for (const FOnlineSessionSearchResult& Result : SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *Result.GetSessionIdStr());
			int32 MaximumPlayers = Result.Session.SessionSettings.NumPublicConnections;
			FServerData Data;

			Data.CurrentPlayers = MaximumPlayers - Result.Session.NumOpenPublicConnections;
			// These are the session settings we set when creating the session
			Data.MaxPlayers = MaximumPlayers;
			Data.HostUsername = Result.Session.OwningUserName;

			// Extract the server name from the sessionsettings, using the Get() and the name settings key
			FString FoundServerName;
			// If get was successful
			if (Result.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, FoundServerName))
			{
				UE_LOG(LogTemp, Warning, TEXT("Data Found in settings: %s"), *FoundServerName);
				Data.ServerName = FoundServerName;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Didn't get expected data in search result session settings"));
				Data.ServerName = "Could Not Find Name";
			}

			DataArray.Add(Data);
		}

		OnServersRefreshed.Broadcast(DataArray);


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find session completed with error"));
	}
}

void USGameInstance::CancelFindSessions()
{
	if (!SessionInterface.IsValid()) { return; }
	if(!SessionSearch.IsValid()) { return; }

	// If we actually have a search that is in progress, then cancel it, otherwise ignore
	if (SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		SessionInterface->CancelFindSessions();
	}
}

void USGameInstance::HandleCancelFindSessionsComplete(bool bWasSuccessful)
{
	// Broadcast that cancel find completed, the Lobby Find screen binds to this, and resets it's state to no longer searching
	OnCancelFindComplete.Broadcast(bWasSuccessful);

	//if (bWasSuccessful)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("GI: Cancel Find Sessions completed SUCCESSFUL"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Cancel Find Sessions completed UNSUCCESSFUL"));
	//}
}