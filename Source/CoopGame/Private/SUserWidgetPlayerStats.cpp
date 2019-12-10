// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetPlayerStats.h"
#include "Components/TextBlock.h"

void USUserWidgetPlayerStats::SetAllText(FString NewName, FString NewKills, FString NewDeath, FString NewScore)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(NewName));
	}
	if (KillsText)
	{
		KillsText->SetText(FText::FromString(NewKills));
	}
	if (DeathText)
	{
		DeathText->SetText(FText::FromString(NewDeath));
	}
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(NewScore));
	}
}
