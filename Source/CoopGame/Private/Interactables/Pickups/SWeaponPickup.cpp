// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "Components/SphereComponent.h"
#include "SPlayerController.h"

ASWeaponPickup::ASWeaponPickup()
{
	WeaponInfo = FWeaponInfo();
	SphereComp->SetSphereRadius(80.f, false);
}

void ASWeaponPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APawn* OverlappedPawn = Cast<APawn>(OtherActor);
	if (!OverlappedPawn) { return; }

	HandlePickupWeapon(OverlappedPawn->GetController());
}

void ASWeaponPickup::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

	HandlePickupWeapon(InteractedPC);
}

void ASWeaponPickup::HandlePickupWeapon(class AController* NewPickupController)
{
	ASPlayerController* PC = Cast<ASPlayerController>(NewPickupController);
	if (!PC) { return; }

	// If requesting controller successfully picks up the weapon, destroy this pickup actor
	if (PC->PickedUpNewWeapon(WeaponInfo))
	{
		Destroy();
	}
}