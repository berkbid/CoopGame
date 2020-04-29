// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCLobby.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API APCLobby : public APlayerController
{
	GENERATED_BODY()
	

public:
	APCLobby();

protected:
	virtual void BeginPlay() override;


private:
	TSubclassOf<class USLobby> wLobby;

	class USLobby* LobbyMenuWidget;


};
