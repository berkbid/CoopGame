// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"


ASProjectileWeapon::ASProjectileWeapon()
{
	RateOfFire = 50.f;
}

//void ASProjectileWeapon::BeginPlay()
//{
//	Super::BeginPlay();
//
//	TimeBetweenShots = 60.f / RateOfFire;
//
//}

void ASProjectileWeapon::Fire()
{
	// So if we are a client, we run ServerFire() to run this Fire function for us
	if (Role < ROLE_Authority)
	{
		ServerFire();
		// Set this on client side since it is checked on client side in StartFire() in SWeapon.cpp
		LastFireTime = GetWorld()->TimeSeconds;

		// If we return here, the owning client will not run this code for itself
		return;
	}

	// This code only gets run on server
	// Owner is player pawn
	AActor* MyOwner = GetOwner();

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

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

		// Sets variable for server to know lastfiretime in case server calls StartFire() incase there is no dedicated server
		LastFireTime = GetWorld()->TimeSeconds;
	}

}
