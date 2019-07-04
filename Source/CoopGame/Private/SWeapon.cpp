// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


// In game console variable for debug weapon drawing
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	// This is defined in P_SmokeTrail under Target > Distribution > Parameter Name
	TracerTargetName = "BeamEnd";

	MuzzleEffect = nullptr;
	ImpactEffect = nullptr;
	TracerEffect = nullptr;
}


void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		// We override the location return in SCharacter.cpp to return camera location instead
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		// Trace against each individual triangle of mesh we are hitting, more expensive trace query
		QueryParams.bTraceComplex = true;

		// Particle "Target" paramter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		// If blocking hit, process damage
		// This line trace starts from camera location and goes in direction of eyerotation, ends up being middle of screen
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			TracerEndPoint = Hit.ImpactPoint;
			
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		
		PlayFireEffect(TracerEndPoint);
	}

	
}

void ASWeapon::PlayFireEffect(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}


	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation("MuzzleSocket");
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}

	}
}


