// Fill out your copyright notice in the Description page of Project Settings.


#include "SLobby.h"
#include "SButtonCustom.h"

bool USLobby::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	// Setup button bindings for 3 main menu buttons
	if (LeaveButton)
	{
		LeaveButton->OnSButtonClicked.RemoveDynamic(this, &USLobby::OnLeaveclicked);
		LeaveButton->OnSButtonClicked.AddDynamic(this, &USLobby::OnLeaveclicked);
	}
	if (StartButton)
	{
		StartButton->OnSButtonClicked.RemoveDynamic(this, &USLobby::OnStartClicked);
		StartButton->OnSButtonClicked.AddDynamic(this, &USLobby::OnStartClicked);
	}

	return bShouldInitialize;
}

void USLobby::OnLeaveclicked()
{
	// Leave session ??
	// client travel to main menu?
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
	}
}

void USLobby::OnStartClicked()
{

}
