// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoPickup.h"

ASAmmoPickup::ASAmmoPickup()
{
	AmmoType = EAmmoType::MiniAmmo;
	AmmoAmount = 30;
}

void ASAmmoPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

}
