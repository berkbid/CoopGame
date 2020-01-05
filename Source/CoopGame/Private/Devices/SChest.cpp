// Fill out your copyright notice in the Description page of Project Settings.


#include "SChest.h"
#include "SWeaponPickup.h"
#include "Engine/World.h"

ASChest::ASChest()
{
	bIsOpened = false;
}

// We are server in here
void ASChest::Interact(AActor* InteractedActor)
{
	Super::Interact(InteractedActor);

	// Only allow chest to be opened once
	if (bIsOpened) { return; }

	bIsOpened = true;

	float HorizontalOffset = -50.f;

	// Decide on weapon type and rarity to spawn
	// Choose a weighted random weapon base class from Array of weapon base classes
	// Then choose rarity -> decide what subclass of weapon base class to spawn

	//spawn items in array
	for (TSubclassOf<ASWeaponPickup> WP : WeaponArray)
	{
		if (WP)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;

			FVector SpawnLocation = GetActorLocation() - FVector(100.f, HorizontalOffset, 0.f);
			GetWorld()->SpawnActor<ASWeaponPickup>(WP, SpawnLocation, FRotator(0.f, 90.f, 0.f), SpawnParams);

			HorizontalOffset += 50.f;
		}
	}

	// Want to call this on clients
	// Set our info widget invisible if it is visible
	//SetInfoInvisible();
}

// We are client in here, checking server variable
void ASChest::ShowItemInfo(bool bIsVisible)
{
	// bIsOpened is replicated so clients should be able to run this
	if (bIsOpened) 
	{ 
		SetInfoInvisible();
		return; 
	}

	Super::ShowItemInfo(bIsVisible);

}

