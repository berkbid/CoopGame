// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventorySlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"


void UWidgetInventorySlot::InitSlot(UTexture2D* WeaponTexture, const FWeaponInfo& NewWeaponInfo)
{
	// Hold weapon info for slot
	CurrentWeaponInfo = NewWeaponInfo;

	// Update slot data, extra ammo doesn't change
	CurrentSlotAmmo = NewWeaponInfo.CurrentAmmo + CurrentExtraAmmo;

	// Set text visible and initial text value
	if (SlotAmmoText)
	{
		SlotAmmoText->SetVisibility(ESlateVisibility::Visible);
		SlotAmmoText->SetText(FText::FromString(FString::FromInt(CurrentSlotAmmo)));
	}

	// Update slot properties
	if (WeaponImage)
	{
		// Set new weapon texture to the slot
		if (WeaponTexture)
		{
			WeaponImage->SetBrushFromTexture(WeaponTexture);
			WeaponImage->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UWidgetInventorySlot::ActivateSlot()
{
	if (ImageBorder)
	{
		ImageBorder->SetBrushColor(FLinearColor(1.f, 1.f, 0.f, 0.75f));
	}

	SetRenderTranslation(FVector2D(0.f, -20.f));
}

void UWidgetInventorySlot::ResetSlot()
{
	if (ImageBorder)
	{
		ImageBorder->SetBrushColor(FLinearColor(1.f, 1.f, 1.f, 0.75f));
	}

	SetRenderTranslation(FVector2D(0.f, 0.f));
}

void UWidgetInventorySlot::UpdateCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	CurrentSlotAmmo = NewCurrentAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

void UWidgetInventorySlot::UpdateExtraAmmo(int32 NewExtraAmmo)
{
	// only update ammo if we have a valid weapon type meaning a valid item in this slot
	if (CurrentWeaponInfo.WeaponClass)
	{
		CurrentExtraAmmo = NewExtraAmmo;
		CurrentSlotAmmo = CurrentWeaponInfo.CurrentAmmo + CurrentExtraAmmo;
		UpdateAmmoText();
	}
}

void UWidgetInventorySlot::UpdateBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	// Update local data
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	CurrentExtraAmmo = NewExtraAmmo;
	CurrentSlotAmmo = NewCurrentAmmo + CurrentExtraAmmo;

	UpdateAmmoText();
}

// Use local data to update text
void UWidgetInventorySlot::UpdateAmmoText()
{
	if (SlotAmmoText)
	{
		SlotAmmoText->SetText(FText::FromString(FString::FromInt(CurrentSlotAmmo)));
	}
}