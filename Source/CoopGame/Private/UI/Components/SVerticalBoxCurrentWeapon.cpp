// Fill out your copyright notice in the Description page of Project Settings.


#include "SVerticalBoxCurrentWeapon.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

USVerticalBoxCurrentWeapon::USVerticalBoxCurrentWeapon()
{

}

void USVerticalBoxCurrentWeapon::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Get reference to our children here for use during play
	CurrentWeaponAmmo = Cast<UOverlay>(GetChildAt(0));
	CurrentWeaponType = Cast<UBorder>(GetChildAt(1));
}

void USVerticalBoxCurrentWeapon::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	CurrentWeaponAmmo = nullptr;
	CurrentWeaponType = nullptr;
	CurrentWeaponInfo.Destroy();

}

// We need this specific data from current slot to initialize our values
void USVerticalBoxCurrentWeapon::InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo = NewWeaponInfo;
	ExtraClipSize = NewExtraAmmo;

	SetAmmoType();
	SetAmmoText();
	SetWeaponText();
}

void USVerticalBoxCurrentWeapon::QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	if (CurrentWeaponInfo.AmmoType == NewAmmoType)
	{
		SetWeaponExtraAmmo(NewExtraAmmo);
	}
}

void USVerticalBoxCurrentWeapon::SetWeaponCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	SetAmmoText();
}

void USVerticalBoxCurrentWeapon::SetWeaponExtraAmmo(int32 NewExtraAmmo)
{
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}

void USVerticalBoxCurrentWeapon::SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}

void USVerticalBoxCurrentWeapon::SetAmmoText()
{
	if (CurrentWeaponAmmo)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CurrentWeaponAmmo->GetChildAt(1));
		if (AmmoText)
		{
			AmmoText->SetText(FText::FromString(FString::FromInt(CurrentWeaponInfo.CurrentAmmo) + " / " + FString::FromInt(ExtraClipSize)));
		}
	}
}

void USVerticalBoxCurrentWeapon::SetWeaponText()
{
	if (CurrentWeaponType)
	{
		UTextBlock* WeaponText = Cast<UTextBlock>(CurrentWeaponType->GetChildAt(0));
		if (WeaponText)
		{
			// If we have weapon in slot use its name, else set it blank
			if (CurrentWeaponInfo.WeaponClass)
			{
				WeaponText->SetText(FText::FromString(CurrentWeaponInfo.WeaponName.ToString()));
			}
			else
			{
				WeaponText->SetText(FText());
			}
			
		}
	}
}

void USVerticalBoxCurrentWeapon::SetAmmoType()
{
	if (!CurrentWeaponAmmo) { return; }
	
	UBorder* AmmoBorder = Cast<UBorder>(CurrentWeaponAmmo->GetChildAt(0));
	if (AmmoBorder)
	{
		// Find texture associated with weapon class we picked up
		UTexture2D** TempWeaponTexture = AmmoToTextureMap.Find(CurrentWeaponInfo.AmmoType);
		if (TempWeaponTexture)
		{
			AmmoBorder->SetVisibility(ESlateVisibility::Visible);
			AmmoBorder->SetBrushFromTexture(*TempWeaponTexture);
		}
		else
		{
			AmmoBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
