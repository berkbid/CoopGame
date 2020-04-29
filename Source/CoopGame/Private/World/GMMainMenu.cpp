// Fill out your copyright notice in the Description page of Project Settings.


#include "GMMainMenu.h"
#include "PCMainMenu.h"

AGMMainMenu::AGMMainMenu()
{
	// Set default PlayerController class to use our custom APCMainMenu
	TSubclassOf<APCMainMenu> PCClass = APCMainMenu::StaticClass();
	if (PCClass)
	{
		PlayerControllerClass = PCClass;
	}

}

void AGMMainMenu::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGMMainMenu::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
