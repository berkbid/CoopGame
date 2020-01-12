// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetWeaponDisplay.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UWidgetWeaponDisplay::InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo = NewWeaponInfo;
	ExtraClipSize = NewExtraAmmo;
	TempCurrentAmmo = CurrentWeaponInfo.CurrentAmmo;

	SetAmmoImage(CurrentWeaponInfo.AmmoType);
	SetAmmoText(TempCurrentAmmo);
	SetWeaponText(CurrentWeaponInfo.WeaponName);
}

void UWidgetWeaponDisplay::SetAmmoText(int32 NewCurrentAmmo)
{
	TempCurrentAmmo = NewCurrentAmmo;
	if (CurrentWeaponAmmo)
	{
		CurrentWeaponAmmo->SetText(FText::FromString(FString::FromInt(NewCurrentAmmo) + " / " + FString::FromInt(ExtraClipSize)));
	}
}

void UWidgetWeaponDisplay::SetWeaponText(FName NewWeaponName)
{
	if (CurrentWeaponText)
	{
		// If we have weapon in slot use its name, else set it blank
		if (CurrentWeaponInfo.WeaponClass)
		{
			CurrentWeaponText->SetText(FText::FromString(NewWeaponName.ToString()));
		}
		else
		{
			CurrentWeaponText->SetText(FText());
		}
	}
}

// Just needs ammo type data from CurrentWeaponInfo
void UWidgetWeaponDisplay::SetAmmoImage(EAmmoType NewAmmoType)
{
	if (!CurrentAmmoBorder) { return; }

	// If there is no weapon equipped, set ammo image hidden
	if (!CurrentWeaponInfo.WeaponClass)
	{
		CurrentAmmoBorder->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	// Find texture associated with weapon class we picked up and set ammo image
	UTexture2D** TempAmmoTexture = AmmoToTextureMap.Find(NewAmmoType);
	if (!TempAmmoTexture) { return; }
	if(UTexture2D* AmmoTexture = *TempAmmoTexture)
	{
		CurrentAmmoBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CurrentAmmoBorder->SetBrushFromTexture(AmmoTexture);
	}
}

void UWidgetWeaponDisplay::QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	if (CurrentWeaponInfo.AmmoType == NewAmmoType)
	{
		ExtraClipSize = NewExtraAmmo;
		SetAmmoText(TempCurrentAmmo);
	}
}