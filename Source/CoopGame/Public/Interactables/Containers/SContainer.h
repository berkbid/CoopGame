// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractable.h"
#include "SContainer.generated.h"

UCLASS()
class COOPGAME_API ASContainer : public ASInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASContainer();

	/* Method for line trace of players to call to show item info on widget component*/
	virtual void ShowItemInfo(bool bIsVisible) override;

	/* Method called when player character tries to interact with this container */
	virtual void Interact(class APlayerController* InteractedPC) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/* Clients can immediately hide the info display for this container since it has been opened */
	UFUNCTION()
	virtual void OnRep_OpenContainer();

	/* Clients need this information when requesting widget info display visibility */
	UPROPERTY(ReplicatedUsing= OnRep_OpenContainer)
	bool bIsOpened;

};
