// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractable.h"
#include "SItemPickup.generated.h"



UCLASS()
class COOPGAME_API ASItemPickup : public ASInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASItemPickup();

	/* Method for line trace of players to call to show item info on widget component*/
	//virtual void ShowItemInfo() override;

	virtual void Interact(AActor* InteractedActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
	UFUNCTION()
	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
