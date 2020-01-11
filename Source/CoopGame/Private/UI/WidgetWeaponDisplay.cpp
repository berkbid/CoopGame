// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetWeaponDisplay.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UWidgetWeaponDisplay::InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo = NewWeaponInfo;
	ExtraClipSize = NewExtraAmmo;

	SetAmmoImage();
	SetAmmoText();
	SetWeaponText();
}

void UWidgetWeaponDisplay::SetWeaponCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	SetAmmoText();
}

void UWidgetWeaponDisplay::SetWeaponExtraAmmo(int32 NewExtraAmmo)
{
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}

void UWidgetWeaponDisplay::SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}

void UWidgetWeaponDisplay::SetAmmoText()
{
	if (CurrentWeaponAmmo)
	{
		CurrentWeaponAmmo->SetText(FText::FromString(FString::FromInt(CurrentWeaponInfo.CurrentAmmo) + " / " + FString::FromInt(ExtraClipSize)));
	}
}

void UWidgetWeaponDisplay::SetWeaponText()
{
	if (CurrentWeaponText)
	{
		// If we have weapon in slot use its name, else set it blank
		if (CurrentWeaponInfo.WeaponClass)
		{
			CurrentWeaponText->SetText(FText::FromString(CurrentWeaponInfo.WeaponName.ToString()));
		}
		else
		{
			CurrentWeaponText->SetText(FText());
		}
	}
}

void UWidgetWeaponDisplay::SetAmmoImage()
{
	if (!CurrentAmmoBorder) { return; }
	
	// Find texture associated with weapon class we picked up
	UTexture2D** TempAmmoTexture = AmmoToTextureMap.Find(CurrentWeaponInfo.AmmoType);
	if (!TempAmmoTexture) { return; }

	if(UTexture2D* AmmoTexture = *TempAmmoTexture)
	{
		CurrentAmmoBorder->SetVisibility(ESlateVisibility::Visible);
		CurrentAmmoBorder->SetBrushFromTexture(AmmoTexture);
	}
}

void UWidgetWeaponDisplay::QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	if (CurrentWeaponInfo.AmmoType == NewAmmoType)
	{
		SetWeaponExtraAmmo(NewExtraAmmo);
	}
}