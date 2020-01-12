// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetScoreboardHUD.h"
#include "WidgetScoreboardEntry.h"
#include "Components/VerticalBox.h"

void UWidgetScoreboardHUD::AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber)
{
	if (ScoreboardEntryClass)
	{
		UWidgetScoreboardEntry* NewPlayerEntry = CreateWidget<UWidgetScoreboardEntry>(this, ScoreboardEntryClass);
		if (NewPlayerEntry && ScoreboardVertical)
		{
			NewPlayerEntry->InitEntry(NewPlayerName);
			ScoreboardVertical->AddChild(NewPlayerEntry);

			// Add reference to PlayerStats to ScoreboardDictionary for updating in the future
			ScoreboardDictionary.Add(NewPlayerNumber, NewPlayerEntry);
		}
	}
}

void UWidgetScoreboardHUD::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	UWidgetScoreboardEntry** PlayerEntry = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerEntry)
	{
		(*PlayerEntry)->SetScoreText(FString::SanitizeFloat(NewScore));
	}
}

void UWidgetScoreboardHUD::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	UWidgetScoreboardEntry** PlayerEntry = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerEntry)
	{
		(*PlayerEntry)->SetKillText(FString::FromInt(NewKills));
	}
}

void UWidgetScoreboardHUD::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	UWidgetScoreboardEntry** PlayerEntry = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerEntry)
	{
		(*PlayerEntry)->SetDeathText(FString::FromInt(NewDeaths));
	}
}