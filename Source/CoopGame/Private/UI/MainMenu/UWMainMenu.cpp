// Fill out your copyright notice in the Description page of Project Settings.


#include "UWMainMenu.h"
#include "Components/WidgetSwitcher.h"
#include "SButtonCustom.h"
#include "LobbyCreate.h"
#include "LobbyFind.h"

bool UUWMainMenu::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	// Setup button bindings for 3 main menu buttons
	if (CreateLobbyButton)
	{
		CreateLobbyButton->OnSButtonClicked.RemoveDynamic(this, &UUWMainMenu::OnCreateLobbyClicked);
		CreateLobbyButton->OnSButtonClicked.AddDynamic(this, &UUWMainMenu::OnCreateLobbyClicked);
	}
	if (FindLobbyButton)
	{
		FindLobbyButton->OnSButtonClicked.RemoveDynamic(this, &UUWMainMenu::OnFindLobbyClicked);
		FindLobbyButton->OnSButtonClicked.AddDynamic(this, &UUWMainMenu::OnFindLobbyClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnSButtonClicked.RemoveDynamic(this, &UUWMainMenu::OnExitClicked);
		ExitButton->OnSButtonClicked.AddDynamic(this, &UUWMainMenu::OnExitClicked);
	}
	// Bind to cancel button inside the lobby create stub
	if (LobbyCreateStub)
	{
		LobbyCreateStub->OnCancelCreate.RemoveDynamic(this, &UUWMainMenu::OnLobbyCreateCancel);
		LobbyCreateStub->OnCancelCreate.AddDynamic(this, &UUWMainMenu::OnLobbyCreateCancel);
	}
	// Bind to cancel button inside the lobby find stub
	if (LobbyFindStub)
	{
		LobbyFindStub->OnJoinCancel.RemoveDynamic(this, &UUWMainMenu::OnJoinLobbyCancel);
		LobbyFindStub->OnJoinCancel.AddDynamic(this, &UUWMainMenu::OnJoinLobbyCancel);
	}

	return bShouldInitialize;
}

void UUWMainMenu::OnCreateLobbyClicked()
{
	if (MenuSwitcher && LobbyCreateStub)
	{
		MenuSwitcher->SetActiveWidget(LobbyCreateStub);
	}
}

void UUWMainMenu::OnLobbyCreateCancel()
{
	if (MenuSwitcher && MainMenuStub)
	{
		MenuSwitcher->SetActiveWidget(MainMenuStub);
	}
}

void UUWMainMenu::OnFindLobbyClicked()
{
	if (MenuSwitcher && LobbyFindStub)
	{
		MenuSwitcher->SetActiveWidget(LobbyFindStub);

		// Tell the lobby find widget that we have started searching for lobbies
		LobbyFindStub->RestartLobbySearch();

	}
}

void UUWMainMenu::OnJoinLobbyCancel()
{
	// Set main menu stub widget to active
	if (MenuSwitcher && MainMenuStub)
	{
		MenuSwitcher->SetActiveWidget(MainMenuStub);
	}
}


void UUWMainMenu::OnExitClicked()
{
	// Use GetWorld()->GetFirstPlayerController();  ??
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->ConsoleCommand("quit");
	}
}


