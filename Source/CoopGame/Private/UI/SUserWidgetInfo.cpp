// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfo.h"
#include "Components/TextBlock.h"


void USUserWidgetInfo::SetOwningActor(AActor* NewOwner)
{
	if (!NewOwner) { return; }
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

	SetSampleText(NewOwner->GetName());
}

void USUserWidgetInfo::SetTextVisibility(bool bIsVisible)
{
	if (SampleText)
	{
		if (bIsVisible)
		{
			SampleText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SampleText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USUserWidgetInfo::SetSampleText(FString PlayerName)
{
	if (SampleText)
	{
		SampleText->SetText(FText::FromString(PlayerName));
	}
}


