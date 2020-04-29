// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyFind.h"
#include "Components/ScrollBox.h"
#include "ServerEntry.h"
#include "UObject/ConstructorHelpers.h"
#include "SButtonCustom.h"
#include "Components/WidgetSwitcher.h"

ULobbyFind::ULobbyFind(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Get class reference of server entry wbp
	static ConstructorHelpers::FClassFinder<UServerEntry> FoundServerEntryClass(TEXT("/Game/UI/Menus/LobbyFind/WBP_ServerEntry"));
	if (FoundServerEntryClass.Class != NULL)
	{
		ServerEntryClass = FoundServerEntryClass.Class;
	}
}

bool ULobbyFind::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	// Bind to the Game Instance delegates when FindSessions completes and CancelFindSessions completes
	GI = Cast<USGameInstance>(GetGameInstance());
	if (GI)
	{
		//UE_LOG(LogTemp, Warning, TEXT("LobbyFind Found game instance"));

		// Bind to when FindSessionsComplete gets called, so we can populate our server list
		GI->OnServersRefreshed.RemoveDynamic(this, &ULobbyFind::RepopulateServerList);
		GI->OnServersRefreshed.AddDynamic(this, &ULobbyFind::RepopulateServerList);

		// Bind to when a CancelFindSessions operation completes, this something we call and we need to handle the callback
		GI->OnCancelFindComplete.RemoveDynamic(this, &ULobbyFind::CancelFindComplete);
		GI->OnCancelFindComplete.AddDynamic(this, &ULobbyFind::CancelFindComplete);
	}
	if (CancelButton)
	{
		CancelButton->OnSButtonClicked.RemoveDynamic(this, &ULobbyFind::OnCancelClicked);
		CancelButton->OnSButtonClicked.AddDynamic(this, &ULobbyFind::OnCancelClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnSButtonClicked.RemoveDynamic(this, &ULobbyFind::OnJoinClicked);
		JoinButton->OnSButtonClicked.AddDynamic(this, &ULobbyFind::OnJoinClicked);
	}
	if (RefreshButton)
	{
		RefreshButton->OnSButtonClicked.RemoveDynamic(this, &ULobbyFind::OnRefreshClicked);
		RefreshButton->OnSButtonClicked.AddDynamic(this, &ULobbyFind::OnRefreshClicked);
	}

	return bShouldInitialize;
}

// This is called from Main Menu or when Refresh button is clicked, which is only showing when no there is no active FindSessions() running
void ULobbyFind::RestartLobbySearch()
{
	// Clear all active servers on the list
	if (ServerListScrollBox)
	{
		ServerListScrollBox->ClearChildren();
	}

	// Want to clear any variable that stores currently selected lobby!

	// Then talk to game instance to refresh server list
	// Need game instance to respond back with array of server information, on FindSessionsComplete() delegate
	// We bind to that delegate in the RepopulateServerList(TArray<FServerData> ServerList) function to handle this callback internally
	if (GI)
	{
		// Set state to currently finding sessions, because if we weren't before this call, we will be after we call GI->FindSessions()
		ChangeToSearching(true);

		// If we could not start finding sessions because already active, then change our state back to not searching
		GI->FindSessions();
	}
}

void ULobbyFind::OnCancelClicked()
{
	// Stop any current lobby searches since we are leaving this widget
	// We still show searching until this callback returns, then we are officially not searching anymore
	StopLobbySearch();

	// Broadcast cancel was clicked, so main menu uses widget switcher to remove us
	OnJoinCancel.Broadcast();
}

void ULobbyFind::StopLobbySearch()
{
	if (GI)
	{
		// Cancel finding sessions
		GI->CancelFindSessions();
	}
}

void ULobbyFind::OnJoinClicked()
{

}

void ULobbyFind::OnRefreshClicked()
{
	RestartLobbySearch();
}

// This is bound to the Game Instance delegate when FindSessions Completes, with the server list data
void ULobbyFind::RepopulateServerList(TArray<FServerData> ServerList)
{
	if(!ServerEntryClass || !ServerListScrollBox) { return; }
	UWorld* World = GetWorld();
	if(!World) { return; }

	// DEBUG FAKE SERVERS //////
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//ServerList.Add(FServerData());
	//////////////////////////////


	//UE_LOG(LogTemp, Warning, TEXT("Lobby Find Repopulating Server List"));
	// Clear any children when we get new search results, even if we are not visible
	if (ServerListScrollBox->HasAnyChildren())
	{
		ServerListScrollBox->ClearChildren();
	}

	// Loop through servers we found and populate our server list
	for (const FServerData& ServerData : ServerList)
	{
		UServerEntry* NewServer = CreateWidget<UServerEntry>(World, ServerEntryClass);
		if (NewServer)
		{
			// Update the new server wbp with some data before adding as child
			//UE_LOG(LogTemp, Warning, TEXT("Found Server: %s"), *ServerData.ServerName);

			// Populate the server entry with its information
			NewServer->SetupServerEntry(ServerData.ServerName, ServerData.HostUsername, ServerData.CurrentPlayers, ServerData.MaxPlayers);

			ServerListScrollBox->AddChild(NewServer);
		}
	}

	// Can safely change searching state to false, since this is called directly from FindSessionsComplete delegate
	ChangeToSearching(false);
}

void ULobbyFind::CancelFindComplete(bool bWasSuccessful)
{
	// When we have finished canceling the find sessions operation, then change state to not searching, so use can restart safely now
	ChangeToSearching(false);
}

// This sets the state of the widget depending if currently searching for lobbies or not, important because should only show Refresh button when not currently searching
void ULobbyFind::ChangeToSearching(bool bIsSearching)
{
	if(!FindLobbySwitcher) { return; }

	// Switch to showing Searching... text
	if (bIsSearching)
	{
		// Display widget that shows we are searching for lobbies
		if (FindingLobbiesText)
		{
			FindLobbySwitcher->SetActiveWidget(FindingLobbiesText);
		}
	}
	// Switch to showing the refresh button
	else
	{
		// Hide searching widget and activate the refresh button widget, since search is now complete and displayed
		if (FindingLobbiesRefresh)
		{
			FindLobbySwitcher->SetActiveWidget(FindingLobbiesRefresh);
		}
	}
}