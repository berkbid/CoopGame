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

// This is client call
void ASContainer::ShowItemInfo(bool bIsVisible)
{
	// Don't show item info if container is already opened, bIsOpened is replicated
	if (bIsOpened) { return; }

	Super::ShowItemInfo(bIsVisible);
}


void ASContainer::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

	// After a container is opened, it can now be dormant and no longer needs to be replicated
	SetNetDormancy(DORM_DormantAll);
}

void ASContainer::OnRep_OpenContainer()
{
	// Only upon container changing to be opened, make sure info is invisible
	// Only clients who have info showing need to run this code, no easy way to determine this
	if (bIsOpened)
	{
		SetInfoInvisible();
	}
}

void ASContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASContainer, bIsOpened);
}

