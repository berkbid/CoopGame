// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerEntry.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"

bool UServerEntry::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	if (EntryButton)
	{
		EntryButton->OnClicked.RemoveDynamic(this, &UServerEntry::OnEntryClicked);
		EntryButton->OnClicked.AddDynamic(this, &UServerEntry::OnEntryClicked);
	}


	return bShouldInitialize;
}


void UServerEntry::OnEntryClicked()
{
	if (EntryBorder)
	{
		EntryBorder->SetBrushColor(FLinearColor(0.f, 1.f, 0.f, 1.f));
	}
}

void UServerEntry::SetupServerEntry(const FString& NewServerName, const FString& NewHostName, int32 NewNumPlayers, int32 NewMaxPlayers)
{
	if (LobbyNameText)
	{
		LobbyNameText->SetText(FText::FromString(NewServerName));
	}
	if (HostNameText)
	{
		HostNameText->SetText(FText::FromString(NewHostName));
	}
	if (IsPrivateText)
	{
		IsPrivateText->SetText(FText::FromString("No"));
	}
	if (PlayersText)
	{
		PlayersText->SetText(FText::FromString(FString::FromInt(NewNumPlayers) + " / " + FString::FromInt(NewMaxPlayers)));
	}
}
