// Fill out your copyright notice in the Description page of Project Settings.

#include "..\Public\SProjectileWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"


void ASProjectileWeapon::Fire()
{
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

	}

}
