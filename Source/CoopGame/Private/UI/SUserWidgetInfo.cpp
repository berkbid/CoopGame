// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfo.h"
#include "Components/TextBlock.h"

void USUserWidgetInfo::SetSampleText(FString PlayerName)
{
	if(SampleText)
	{
		SampleText->SetText(FText::FromString(PlayerName));
	}
}

void USUserWidgetInfo::SetTextVisibility()
{
	if (SampleText)
	{
		SampleText->SetVisibility(ESlateVisibility::Visible);
	}
}
