// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCreate.h"
#include "SButtonCustom.h"
#include "Components/EditableText.h"
#include "SGameInstance.h"

bool ULobbyCreate::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	// Setup button bindings for 3 main menu buttons
	if (CancelButton)
	{
		CancelButton->OnSButtonClicked.RemoveDynamic(this, &ULobbyCreate::OnCancelclicked);
		CancelButton->OnSButtonClicked.AddDynamic(this, &ULobbyCreate::OnCancelclicked);
	}
	if (Createbutton)
	{
		Createbutton->OnSButtonClicked.RemoveDynamic(this, &ULobbyCreate::OnCreateClicked);
		Createbutton->OnSButtonClicked.AddDynamic(this, &ULobbyCreate::OnCreateClicked);
	}

	return bShouldInitialize;
}

void ULobbyCreate::OnCancelclicked()
{
	// Broadcast this for main menu to widget switch back
	OnCancelCreate.Broadcast();
}

void ULobbyCreate::OnCreateClicked()
{
	if (!LobbyName) { return; }
	
	//UE_LOG(LogTemp, Warning, TEXT("Found Lobby Name: %s"), *LobbyName->GetText().ToString());

	USGameInstance* GI = Cast<USGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->CreateSession(LobbyName->GetText().ToString());
	}
}