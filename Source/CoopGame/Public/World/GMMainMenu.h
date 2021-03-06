// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GMMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API AGMMainMenu : public AGameMode
{
	GENERATED_BODY()
	

public:

	AGMMainMenu();

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	void Logout(AController* Exiting) override;
};
