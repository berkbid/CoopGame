// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GMLobby.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API AGMLobby : public AGameMode
{
	GENERATED_BODY()
	

public:

	AGMLobby();

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed */
	void Logout(AController* Exiting) override;

private:
	void TimeToGoToGame();
};
