// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"

ASWeaponPickup::ASWeaponPickup()
{
	WeaponInfo = FWeaponInfo();
	SphereComp->SetSphereRadius(70.f, false);
}

void ASWeaponPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	RequestPickupWeapon(OtherActor);
}

void ASWeaponPickup::Interact(AActor* InteractedActor)
{
	Super::Interact(InteractedActor);

	RequestPickupWeapon(InteractedActor);
	
}

void ASWeaponPickup::RequestPickupWeapon(AActor* PickupActor)
{
	if (WeaponInfo.WeaponType)
	{
		ASCharacter* OverlappedCharacter = Cast<ASCharacter>(PickupActor);
		if (OverlappedCharacter)
		{
			OverlappedCharacter->PickupWeapon(WeaponInfo, this);
		}
	}
}
