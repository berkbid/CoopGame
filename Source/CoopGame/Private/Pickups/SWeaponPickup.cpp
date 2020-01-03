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
	if (WeaponInfo.WeaponType)
	{
		ASCharacter* OverlappedCharacter = Cast<ASCharacter>(OtherActor);
		if (OverlappedCharacter)
		{
			OverlappedCharacter->PickupWeapon(WeaponInfo, this);
		}
	}
}
