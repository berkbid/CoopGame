// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API APCMainMenu : public APlayerController
{
	GENERATED_BODY()
	

public:
	APCMainMenu();

protected:
	virtual void BeginPlay() override;

private:
	TSubclassOf<class UUWMainMenu> wMainMenu;

	class UUWMainMenu* MainMenuWidget;
};
