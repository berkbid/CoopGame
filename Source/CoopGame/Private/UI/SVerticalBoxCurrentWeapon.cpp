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

void USVerticalBoxCurrentWeapon::SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo.CurrentAmmo = NewCurrentAmmo;
	ExtraClipSize = NewExtraAmmo;
	SetAmmoText();
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
			WeaponText->SetText(FText::FromString(CurrentWeaponInfo.WeaponName.ToString()));
		}
	}
}

void USVerticalBoxCurrentWeapon::InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	CurrentWeaponInfo = NewWeaponInfo;
	ExtraClipSize = NewExtraAmmo;

	SetAmmoText();
	SetWeaponText();
}
