// Fill out your copyright notice in the Description page of Project Settings.


#include "SOverlayInventorySlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"
#include "Engine/Texture2D.h"


USOverlayInventorySlot::USOverlayInventorySlot()
{
	CurrentWeaponInfo = FWeaponInfo();
	CurrentSlotAmmo = 0;
	CurrentExtraAmmo = 0;
}

void USOverlayInventorySlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SlotBorder = nullptr;
	SlotImage = nullptr;
	AmmoText = nullptr;
	CurrentWeaponInfo.Destroy();
}

void USOverlayInventorySlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Get reference to our children here for use during play
	SlotBorder = Cast<UBorder>(GetChildAt(0));
	SlotImage = Cast<UImage>(GetChildAt(1));
	AmmoText = Cast<UTextBlock>(GetChildAt(2));
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

// Update local data then update text
void USOverlayInventorySlot::UpdateExtraAmmo(int32 NewExtraAmmo)
{
	// only update ammo if we have a valid weapon type meaning a valid item in this slot
	if (CurrentWeaponInfo.WeaponType)
	{
		CurrentExtraAmmo = NewExtraAmmo;
		CurrentSlotAmmo = CurrentWeaponInfo.CurrentAmmo + CurrentExtraAmmo;
		UpdateAmmoText();
	}
}

// Update local data then update text
void USOverlayInventorySlot::UpdateBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	// Update local data
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	CurrentExtraAmmo = NewExtraAmmo;
	CurrentSlotAmmo = NewCurrentAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

// Update local data then update text
void USOverlayInventorySlot::UpdateCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	CurrentSlotAmmo = NewCurrentAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

// This slot is being un-equipped
void USOverlayInventorySlot::ResetSlot()
{
	if (SlotBorder)
	{
		SlotBorder->SetVisibility(ESlateVisibility::Hidden);
	}

	SetRenderTranslation(FVector2D(0.f, 0.f));
}

// This slot is being equipped
void USOverlayInventorySlot::ActivateSlot()
{
	if (SlotBorder)
	{
		SlotBorder->SetVisibility(ESlateVisibility::Visible);
	}

	SetRenderTranslation(FVector2D(0.f, -20.f));
}

// Slot just equipped new weapon, set children's data
void USOverlayInventorySlot::UpdateWeaponInfo(UTexture2D* WeaponTexture, const FWeaponInfo &NewWeaponInfo)
{
	// Hold weapon info for slot
	CurrentWeaponInfo = NewWeaponInfo;
	
	// Update slot data, extra ammo doesn't change
	CurrentSlotAmmo = NewWeaponInfo.CurrentAmmo + CurrentExtraAmmo;
	
	// Set text visible and initial text value
	if (AmmoText)
	{
		AmmoText->SetVisibility(ESlateVisibility::Visible);
		AmmoText->SetText(FText::FromString(FString::FromInt(CurrentSlotAmmo)));
	}

	// Update slot properties
	if (SlotImage)
	{
		// Set new weapon texture to the slot
		if (WeaponTexture)
		{
			SlotImage->SetBrushFromTexture(WeaponTexture);
			SlotImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void USOverlayInventorySlot::GetWeaponInfo(FWeaponInfo& CopyWeaponInfo, int32& SlotExtraAmmo)
{
	CopyWeaponInfo = CurrentWeaponInfo;
	SlotExtraAmmo = CurrentExtraAmmo;
}

// Use local data to update text
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


