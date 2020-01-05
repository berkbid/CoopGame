// Fill out your copyright notice in the Description page of Project Settings.


#include "SContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CoopGame.h"
#include "SWidgetCompPickupInfo.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASContainer::ASContainer()
{
	BoxComp->SetSimulatePhysics(false);
}

void ASContainer::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
}

// Called when the game starts or when spawned
void ASContainer::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASContainer::Interact(AActor* InteractedActor)
{
	Super::Interact(InteractedActor);

}

void ASContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASContainer, bIsOpened);
}

