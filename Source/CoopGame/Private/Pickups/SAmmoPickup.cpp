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
	
	ASCharacter* OverlappedCharacter = Cast<ASCharacter>(OtherActor);
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
