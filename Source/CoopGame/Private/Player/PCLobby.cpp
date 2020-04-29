// Fill out your copyright notice in the Description page of Project Settings.


#include "PCLobby.h"
#include "SLobby.h"
#include "UObject/ConstructorHelpers.h"

APCLobby::APCLobby()
{
	// Get class reference of WBP_MainMenu
	static ConstructorHelpers::FClassFinder<USLobby> LobbyWidgetClass(TEXT("/Game/UI/Menus/Lobby/WBP_Lobby"));
	if (LobbyWidgetClass.Class != NULL)
	{
		wLobby = LobbyWidgetClass.Class;
		//UE_LOG(LogTemp, Warning, TEXT("Found Main Menu WBP Class: %s"), *MenuClass->GetName());
	}
}

void APCLobby::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Begin Play PC Lobby"));
	// This means server does not run this code unless its a listen server
	if (IsLocalController())
	{
		if (wLobby)
		{
			LobbyMenuWidget = CreateWidget<USLobby>(this, wLobby);

			if (LobbyMenuWidget)
			{
				// Pass reference of ourself to widget while calling setup logic on widget
				LobbyMenuWidget->AddToViewport();

				//UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MainMenuWidget);
				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(LobbyMenuWidget->TakeWidget());
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputModeData);

				bShowMouseCursor = true;
			}
		}
	}
}