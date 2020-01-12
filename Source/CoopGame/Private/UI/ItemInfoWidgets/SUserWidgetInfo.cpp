// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void USUserWidgetInfo::SetOwningActor(AActor* NewOwner)
{
	if (!NewOwner) { return; }
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

}

void USUserWidgetInfo::SetWidgetVisibility(bool bVisible)
{
	if (InfoBorder)
	{
		if (bVisible)
		{
			InfoBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			InfoBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}