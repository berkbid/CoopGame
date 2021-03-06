// Fill out your copyright notice in the Description page of Project Settings.


#include "PCMainMenu.h"
#include "UWMainMenu.h"
#include "UObject/ConstructorHelpers.h"

APCMainMenu::APCMainMenu()
{
	// Get class reference of WBP_MainMenu
	static ConstructorHelpers::FClassFinder<UUWMainMenu> MenuWidgetClass(TEXT("/Game/UI/Menus/MainMenu/WBP_MainMenu"));
	if (MenuWidgetClass.Class != NULL)
	{
		wMainMenu = MenuWidgetClass.Class;
		//UE_LOG(LogTemp, Warning, TEXT("Found Main Menu WBP Class: %s"), *MenuClass->GetName());
	}
}

void APCMainMenu::BeginPlay()
{
	Super::BeginPlay();

	// This means server does not run this code unless its a listen server
	if (IsLocalController())
	{
		if (wMainMenu)
		{
			MainMenuWidget = CreateWidget<UUWMainMenu>(this, wMainMenu);

			if (MainMenuWidget)
			{
				// Pass reference of ourself to widget while calling setup logic on widget
				MainMenuWidget->AddToViewport();

				FInputModeUIOnly InputModeData;
				InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				SetInputMode(InputModeData);

				bShowMouseCursor = true;

			}
		}
	}

}
