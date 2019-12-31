// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "SCharacter.h"

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
