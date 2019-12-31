// Fill out your copyright notice in the Description page of Project Settings.


#include "SVerticalBoxCurrentWeapon.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

USVerticalBoxCurrentWeapon::USVerticalBoxCurrentWeapon()
{

}

void USVerticalBoxCurrentWeapon::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	CurrentWeaponAmmo = nullptr;
	CurrentWeaponType = nullptr;
}

void USVerticalBoxCurrentWeapon::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Get reference to our children here for use during play
	CurrentWeaponAmmo = Cast<UOverlay>(GetChildAt(0));
	CurrentWeaponType = Cast<UBorder>(GetChildAt(1));
}

void USVerticalBoxCurrentWeapon::SetWeaponName(FString NewWeaponName)
{
	if (CurrentWeaponType)
	{
		UTextBlock* WeaponText = Cast<UTextBlock>(CurrentWeaponType->GetChildAt(0));
		if (WeaponText)
		{
			WeaponText->SetText(FText::FromString(NewWeaponName));
		}
	}
}

void USVerticalBoxCurrentWeapon::SetAmmoText()
{
	if (CurrentWeaponAmmo)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CurrentWeaponAmmo->GetChildAt(1));
		if (AmmoText)
		{
			AmmoText->SetText(FText::FromString(FString::FromInt(CurrentClipSize) + " / " + FString::FromInt(ExtraClipSize)));
		}
	}
}

void USVerticalBoxCurrentWeapon::SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	CurrentClipSize = NewCurrentAmmo;
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}

void USVerticalBoxCurrentWeapon::SetWeaponCurrentAmmo(int32 NewCurrentAmmo)
{
	CurrentClipSize = NewCurrentAmmo;
	SetAmmoText();
}

void USVerticalBoxCurrentWeapon::SetWeaponExtraAmmo(int32 NewExtraAmmo)
{
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
}


