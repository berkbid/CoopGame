// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetPlayerStats.h"
#include "Components/TextBlock.h"

void USUserWidgetPlayerStats::SetAllText(FString NewName, FString NewKills, FString NewDeath, FString NewScore)
{
	SetNameText(NewName);
	SetKillText(NewKills);
	SetDeathText(NewDeath);
	SetScoreText(NewScore);
}

void USUserWidgetPlayerStats::SetNameText(FString NewName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(NewName));
	}
}

void USUserWidgetPlayerStats::SetScoreText(FString NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(NewScore));
	}
}

void USUserWidgetPlayerStats::SetKillText(FString NewKills)
{
	if (KillsText)
	{
		KillsText->SetText(FText::FromString(NewKills));
	}
}

void USUserWidgetPlayerStats::SetDeathText(FString NewDeath)
{
	if (DeathText)
	{
		DeathText->SetText(FText::FromString(NewDeath));
	}
}
