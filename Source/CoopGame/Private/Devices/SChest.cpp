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

	// Tell clients this chest has been opened
	bIsOpened = true;
	OnRep_OpenContainer();

	float HorizontalOffset = -50.f;

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
}

void ASChest::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
}

void ASChest::OnRep_OpenContainer()
{
	Super::OnRep_OpenContainer();
}