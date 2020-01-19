// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventorySlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "SWeapon.h"


void UWidgetInventorySlot::InitSlot(const FWeaponInfo& NewWeaponInfo, const TMap<TSubclassOf<ASWeapon>, UTexture2D*>& WeaponTextureMapRef, const TMap<EAmmoType, UTexture2D*>& AmmoTextureMapRef)
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
		// Find texture associated with weapon class we picked up
		UTexture2D* const* WeaponTextureTemp = WeaponTextureMapRef.Find(NewWeaponInfo.WeaponClass);
		if (!WeaponTextureTemp) { return; }

		// Store local pointer to current weapon texture
		CurrentWeaponTexture = *WeaponTextureTemp;

		// Set new weapon texture to the slot
		if (CurrentWeaponTexture)
		{
			WeaponImage->SetBrushFromTexture(CurrentWeaponTexture);
			WeaponImage->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// Update slot ammo image using texture mapping
	if (SlotAmmoImage)
	{
		// Find texture associated with weapon class we picked up and set ammo image
		UTexture2D* const* TempAmmoTexture = AmmoTextureMapRef.Find(CurrentWeaponInfo.AmmoType);
		if (!TempAmmoTexture) { return; }

		// Store local pointer to current ammo texture
		CurrentAmmoTexture = *TempAmmoTexture;
		if (CurrentAmmoTexture)
		{
			SlotAmmoImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SlotAmmoImage->SetBrushFromTexture(CurrentAmmoTexture);
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