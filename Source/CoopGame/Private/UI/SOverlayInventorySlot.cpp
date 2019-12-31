// Fill out your copyright notice in the Description page of Project Settings.


#include "SOverlayInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Engine/Texture2D.h"
#include "SPlayerController.h"


USOverlayInventorySlot::USOverlayInventorySlot()
{

}

void USOverlayInventorySlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SlotBorder = nullptr;
	AmmoText = nullptr;
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

// Updates text automatically after extra ammo is set
void USOverlayInventorySlot::UpdateExtraAmmo(int32 NewExtraAmmo)
{
	// Update local slot data
	CurrentExtraAmmo = NewExtraAmmo;
	CurrentSlotAmmo = CurrentClipAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

void USOverlayInventorySlot::UpdateBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	CurrentClipAmmo = NewCurrentAmmo;
	CurrentExtraAmmo = NewExtraAmmo;
	CurrentSlotAmmo = CurrentClipAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

void USOverlayInventorySlot::UpdateCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentClipAmmo = NewCurrentAmmo;
	CurrentSlotAmmo = CurrentClipAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

// This slot is being un-equipped
void USOverlayInventorySlot::ResetSlot()
{
	SetRenderTranslation(FVector2D(0.f, 0.f));
}

// This slot is being equipped
void USOverlayInventorySlot::ActivateSlot()
{
	SetRenderTranslation(FVector2D(0.f, -20.f));
}

// Slot just equipped new weapon, set children's data
void USOverlayInventorySlot::InitSlot(UTexture2D* WeaponTexture, int32 CurrentAmmo, int32 ExtraAmmo, EAmmoType AmmoTypeNew)
{
	// Update slot data for ourselves
	CurrentAmmoType = AmmoTypeNew;
	CurrentClipAmmo = CurrentAmmo;
	CurrentExtraAmmo = ExtraAmmo;
	CurrentSlotAmmo = CurrentAmmo + ExtraAmmo;
	

	// Set text visible and initial text value
	if (AmmoText)
	{
		AmmoText->SetVisibility(ESlateVisibility::Visible);
		AmmoText->SetText(FText::FromString(FString::FromInt(CurrentSlotAmmo)));
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

void USOverlayInventorySlot::UpdateAmmoText()
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::FromInt(CurrentSlotAmmo)));
	}
}

TSharedRef<SWidget> USOverlayInventorySlot::RebuildWidget()
{
	return Super::RebuildWidget();
}


