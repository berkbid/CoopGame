// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoPickup.h"
#include "SCharacter.h"

ASAmmoPickup::ASAmmoPickup()
{

	AmmoType = EAmmoType::MiniAmmo;
	AmmoAmount = 30;
}

void ASAmmoPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	RequestPickupAmmo(OtherActor);
}

void ASAmmoPickup::Interact(AActor* InteractedActor)
{
	Super::Interact(InteractedActor);
	RequestPickupAmmo(InteractedActor);
}

void ASAmmoPickup::RequestPickupAmmo(AActor* PickupActor)
{
	ASCharacter* OverlappedCharacter = Cast<ASCharacter>(PickupActor);
	if (OverlappedCharacter)
	{
		int32 LeftOverAmmo = OverlappedCharacter->PickupAmmo(AmmoType, AmmoAmount);
		if (LeftOverAmmo <= 0)
		{
			Destroy();
		}
		else
		{
			AmmoAmount = LeftOverAmmo;
		}
	}
}
