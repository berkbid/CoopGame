// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfoAmmo.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "SAmmoPickup.h"
#include "Engine/Texture2D.h"

void USUserWidgetInfoAmmo::SetOwningActor(AActor* NewOwner)
{
	Super::SetOwningActor(NewOwner);

	if (NewOwner)
	{
		ASAmmoPickup* AP = Cast<ASAmmoPickup>(NewOwner);
		if (AP)
		{
			SetAmmoText(AP->AmmoAmount);

			UTexture2D** TempWeaponTexture = AmmoToTextureMap.Find(AP->AmmoType);
			if (TempWeaponTexture)
			{
				SetAmmoImage(*TempWeaponTexture);
			}
		}
	}
}

void USUserWidgetInfoAmmo::SetAmmoText(int32 NewAmmo)
{
	if (AmmoAmountText)
	{
		AmmoAmountText->SetText(FText::FromString("x" + FString::FromInt(NewAmmo)));
	}
}

void USUserWidgetInfoAmmo::SetAmmoImage(UTexture2D* NewTexture)
{
	if (AmmoImage && NewTexture)
	{
		AmmoImage->SetBrushFromTexture(NewTexture);
	}
}
