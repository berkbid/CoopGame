// Fill out your copyright notice in the Description page of Project Settings.


#include "PCMainMenu.h"
#include "UWMainMenu.h"
#include "BluePrint/WidgetBlueprintLibrary.h"

void APCMainMenu::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (wMainMenu)
		{
			MainMenuWidget = CreateWidget<UUWMainMenu>(this, wMainMenu);
			if (MainMenuWidget)
			{
				// Pass reference of ourself to widget while calling setup logic on widget
				//MainMenuWidget->SetOwningController(this);
				MainMenuWidget->AddToViewport();
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MainMenuWidget);
				bShowMouseCursor = true;

			}
		}
	}

}
