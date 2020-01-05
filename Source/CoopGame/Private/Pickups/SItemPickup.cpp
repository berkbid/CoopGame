// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemPickup.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "CoopGame.h"
#include "SWidgetCompPickupInfo.h"

// Sets default values
ASItemPickup::ASItemPickup()
{
	// Set custom collision responses for item pickups
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
}
// Called when the game starts or when spawned
void ASItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Only hook overlap event on server
	if (GetLocalRole() == ROLE_Authority)
	{
		// Bind to overlap event on server because functionality needs to be run on server
		OnActorBeginOverlap.AddDynamic(this, &ASItemPickup::HandleBeginOverlap);
	}
}

void ASItemPickup::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

}

// Only server hooks onto this event
void ASItemPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}