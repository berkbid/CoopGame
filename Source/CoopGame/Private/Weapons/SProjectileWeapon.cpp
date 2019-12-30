// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "SPlayerController.h"

ASProjectileWeapon::ASProjectileWeapon()
{
	// Setup weapon state
	RateOfFire = 240.f;
	TimeBetweenShots = 60.f / RateOfFire;

	CurrentClipSize = -1;
	MaxClipSize = -1;
	AmmoType = EAmmoType::RocketAmmo;
}

void ASProjectileWeapon::Fire()
{
	// So if we are a client, we run ServerFire() to run this Fire function for us
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
		// Set this on client side in case he needs this information
		LastFireTime = GetWorld()->TimeSeconds;

		return;
	}

	// This code only gets run on server
	// Owner is player pawn
	AActor* MyOwner = GetOwner();

	if (CurrentClipSize <= 0) { return; }

	if (MyOwner && ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		// We override the location return in SCharacter.cpp to return camera location instead
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		// we use EyeRotation instead in SpawnActor below
		//FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// Important to set instigator to this actor's owner, aka the player, in order for damage done to be attributed to proper player
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.Owner = GetOwner();

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

		// Sets variable for server to know lastfiretime in case server calls StartFire() incase there is no dedicated server
		LastFireTime = GetWorld()->TimeSeconds;

		CurrentClipSize--;
		OnRep_ClipSize();

		// Update ammo count in player controller as server
		APawn* MyPawnOwner = Cast<APawn>(MyOwner);
		if (MyPawnOwner)
		{
			ASPlayerController* PC = Cast<ASPlayerController>(MyPawnOwner->GetController());
			if (PC)
			{
				PC->DecrementAmmoType(AmmoType, CurrentClipSize);
			}
		}
	}
}
