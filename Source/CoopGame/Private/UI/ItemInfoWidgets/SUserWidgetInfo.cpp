// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfo.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"

void USUserWidgetInfo::SetOwningActor(AActor* NewOwner)
{
	if (!NewOwner) { return; }
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

}

void USUserWidgetInfo::SetWidgetVisibility(bool bVisible)
{
	if (InfoBox)
	{
		if (bVisible)
		{
			InfoBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			InfoBox->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}