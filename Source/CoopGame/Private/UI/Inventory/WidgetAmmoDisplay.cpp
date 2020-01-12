// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetAmmoDisplay.h"
#include "Components/TextBlock.h"

void UWidgetAmmoDisplay::SetAmmoText(FText NewAmmoText)
{
	if (AmmoTextBlock)
	{
		AmmoTextBlock->SetText(NewAmmoText);
	}
}
