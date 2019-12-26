// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"
#include "SUserWidgetPlayerStats.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "GameFramework/PlayerState.h"

bool USUserWidgetGameInfo::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

// Player controller calls this after creating this widget and passes reference to itself
void USUserWidgetGameInfo::SetOwningController(APlayerController* NewController)
{
	// Skip repeated calls
	if (OwningController == NewController) { return; }
	OwningController = NewController;
}

void USUserWidgetGameInfo::SetStateText(FString NewState)
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(NewState));
	}
}

void USUserWidgetGameInfo::AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber)
{
	if (wPlayerStats)
	{
		USUserWidgetPlayerStats* NewPlayerStats = CreateWidget<USUserWidgetPlayerStats>(this, wPlayerStats);
		if (NewPlayerStats && ScoreboardEntryBox)
		{
			NewPlayerStats->SetAllText(NewPlayerName, FString("0"), FString("0"), FString("0"));
			ScoreboardEntryBox->AddChild(NewPlayerStats);
			
			// Add reference to PlayerStats to ScoreboardDictionary for updating in the future
			ScoreboardDictionary.Add(NewPlayerNumber, NewPlayerStats);
		}
	}
}

void USUserWidgetGameInfo::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetScoreText(FString::SanitizeFloat(NewScore));
	}
}

void USUserWidgetGameInfo::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetKillText(FString::FromInt(NewKills));
	}
}

void USUserWidgetGameInfo::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetDeathText(FString::FromInt(NewDeaths));
	}
}

void USUserWidgetGameInfo::HandleScoreChanged(float NewScore)
{
	if (ScoreText)
	{
		FString tempString("SCORE: " + FString::SanitizeFloat(NewScore));
		ScoreText->SetText(FText::FromString(tempString));
	}
}

// HUD actions to represent de-selecting an old inventory slot
void USUserWidgetGameInfo::ResetOldInventorySlot()
{
	if (!CurrentOverlay) { return; }
	
	if (UBorder* TempBorder = Cast<UBorder>(CurrentOverlay->GetChildAt(0)))
	{
		TempBorder->SetBrushColor(FColor::White);
	}

	CurrentOverlay->SetRenderTranslation(FVector2D(0.f, 0.f));
}

// HUD actions to represent selecting a new inventory slot
void USUserWidgetGameInfo::UpdateNewInventorySlot(UOverlay* NewOverlay)
{
	if (!NewOverlay) { return; }

	if (UBorder* TempBorder = Cast<UBorder>(NewOverlay->GetChildAt(0)))
	{
		TempBorder->SetBrushColor(FColor::Blue);
	}

	NewOverlay->SetRenderTranslation(FVector2D(0.f, -20.f));
	CurrentOverlay = NewOverlay;
}

// Called on PlayerController when server updates variable of CurrentSlot, triggers OnRep to owning client to call this code
void USUserWidgetGameInfo::UpdateInventoryHUD(int WeaponSlot)
{
	switch (WeaponSlot)
	{
		case 0:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FirstOverlay);
			break;

		case 1:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(SecondOverlay);
			break;
		case 2:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(ThirdOverlay);
			break;
		case 3:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FourthOverlay);
			break;
		case 4:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FifthOverlay);
			break;
	default:
		break;
	}
}

