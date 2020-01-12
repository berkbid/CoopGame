// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetScoreboardEntry.h"
#include "Components/TextBlock.h"

void UWidgetScoreboardEntry::InitEntry(FString NewPlayerName)
{
	SetNameText(NewPlayerName);
	SetKillText("0");
	SetDeathText("0");
	SetScoreText("0");
}

void UWidgetScoreboardEntry::SetNameText(FString NewName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(NewName));
	}
}

void UWidgetScoreboardEntry::SetScoreText(FString NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(NewScore));
	}
}

void UWidgetScoreboardEntry::SetKillText(FString NewKills)
{
	if (KillsText)
	{
		KillsText->SetText(FText::FromString(NewKills));
	}
}

void UWidgetScoreboardEntry::SetDeathText(FString NewDeath)
{
	if (DeathText)
	{
		DeathText->SetText(FText::FromString(NewDeath));
	}
}