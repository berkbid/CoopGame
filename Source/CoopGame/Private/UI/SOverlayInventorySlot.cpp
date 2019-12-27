// Fill out your copyright notice in the Description page of Project Settings.


#include "SOverlayInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Engine/Texture2D.h"


USOverlayInventorySlot::USOverlayInventorySlot()
{

}

void USOverlayInventorySlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Get reference to our children here for use during play
	SlotBorder = Cast<UBorder>(GetChildAt(0));
	AmmoText = Cast<UTextBlock>(GetChildAt(1));
}


void USOverlayInventorySlot::PostInitProperties()
{
	Super::PostInitProperties();


	//SlotBorder = NewObject<UBorder>(this, UBorder::StaticClass());
	//AddChildToOverlay(SlotBorder);
	//
	//AmmoText = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	//AmmoText->SetText(FText::FromString(FString("40")));
	//AmmoText->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
	//AddChildToOverlay(AmmoText);
}

// This slot is being un-equipped
void USOverlayInventorySlot::ResetSlot()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FColor::White);
	}
	SetRenderTranslation(FVector2D(0.f, 0.f));
}

// This slot is being equipped
void USOverlayInventorySlot::ActivateSlot()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FColor::Blue);
	}
	SetRenderTranslation(FVector2D(0.f, -20.f));
}

// Slot just equipped new weapon, set children's data
void USOverlayInventorySlot::InitSlot(UTexture2D* WeaponTexture)
{
	// Set text visible and initial text value
	if (AmmoText)
	{
		AmmoText->SetVisibility(ESlateVisibility::Visible);
	}

	// Update slot properties
	if (SlotBorder)
	{
		// Set new weapon texture to the slot
		if (WeaponTexture)
		{
			SlotBorder->SetBrushFromTexture(WeaponTexture);
		}
	}
}

TSharedRef<SWidget> USOverlayInventorySlot::RebuildWidget()
{
	return Super::RebuildWidget();
}


