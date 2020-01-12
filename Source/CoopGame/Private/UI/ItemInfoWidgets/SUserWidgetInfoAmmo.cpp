// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfoAmmo.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "SAmmoPickup.h"
#include "Engine/Texture2D.h"

void USUserWidgetInfoAmmo::SetOwningActor(AActor* NewOwner)
{
	Super::SetOwningActor(NewOwner);
	if (!NewOwner) { return; }

	if (ASAmmoPickup* AP = Cast<ASAmmoPickup>(NewOwner))
	{
		SetAmmoText(AP->AmmoAmount);
		SetAmmoImage(AP->AmmoType);
	}
}


void USUserWidgetInfoAmmo::SetAmmoText(int32 NewAmmo)
{
	if (AmmoAmountText)
	{
		AmmoAmountText->SetText(FText::FromString("x" + FString::FromInt(NewAmmo)));
	}
}

void USUserWidgetInfoAmmo::SetFullState()
{
	if (AmmoFullText)
	{
		AmmoFullText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (PickupText)
	{
		PickupText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USUserWidgetInfoAmmo::SetNotFullState()
{
	if (AmmoFullText)
	{
		AmmoFullText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (PickupText)
	{
		PickupText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void USUserWidgetInfoAmmo::SetAmmoImage(EAmmoType NewAmmoType)
{
	if (!AmmoImage) { return; }

	UTexture2D** TempAmmoTexture = AmmoToTextureMap.Find(NewAmmoType);
	if (!TempAmmoTexture) { return; }
	
	if (UTexture2D* NewAmmoTexture = *TempAmmoTexture)
	{
		AmmoImage->SetBrushFromTexture(NewAmmoTexture);
	}
}