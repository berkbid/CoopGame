// Fill out your copyright notice in the Description page of Project Settings.


#include "SRifle.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"


ASRifle::ASRifle()
{
	MuzzleEffect = nullptr;
	DefaultImpactEffect = nullptr;
	TracerEffect = nullptr;
	BulletSpread = 2.f;

	// Setup weapon stats
	BaseDamage = 25.f;
	HeadShotMultiplier = 4.f;
	MaxClipSize = 40;
	//CurrentClipSize = MaxClipSize;
	CurrentClipSize = -1;

	//WeaponInfo.SetMaxAmmo(40);
	//WeaponInfo.SetCurrentAmmo(40);
	//WeaponInfo.SetWeaponType(ASRifle::StaticClass());

	// This is defined in P_SmokeTrail under Target > Distribution > Parameter Name
	TracerTargetName = "BeamEnd";
}

void ASRifle::Fire()
{
	// So if we are a client, we run ServerFire() instead of this Fire() function
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	// If we don't have an owner, don't try to process Fire request
	AActor* MyOwner = GetOwner();
	if (!MyOwner) { return; }

	// If we are out of ammo, return
	if (CurrentClipSize <= 0) { return; }

	FVector EyeLocation;
	FRotator EyeRotation;
	// We override the location return in SCharacter.cpp to return camera location instead
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();

	// Bullet Spread
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

	FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	// Trace against each individual triangle of mesh we are hitting, more expensive trace query
	QueryParams.bTraceComplex = true;
	// Need to set this to true in order to get the type of physical material we hit in order to act appropriately w/ effects, etc
	QueryParams.bReturnPhysicalMaterial = true;

	// Particle "Target" paramter
	FVector TracerEndPoint = TraceEnd;

	EPhysicalSurface SurfaceType = SurfaceType_Default;

	FHitResult Hit;
	// If blocking hit, process damage
	// This line trace starts from camera location and goes in direction of eyerotation, ends up being middle of screen, collision_weapon defined in CoopGame.h
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();
		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			ActualDamage *= HeadShotMultiplier;
		}

		UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

		PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

		TracerEndPoint = Hit.ImpactPoint;

	}

	APawn* MyPawnOwner = Cast<APawn>(MyOwner);
	if (MyPawnOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyPawnOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

	PlayFireEffect(TracerEndPoint);

	// This will replicate the struct HitScanTrace to all clients triggering OnRep function
	if (GetLocalRole() == ROLE_Authority)
	{
		HitScanTrace.TraceTo = TracerEndPoint;
		// Only overriden from default if we hit something
		HitScanTrace.SurfaceType = SurfaceType;
		HitScanTrace.ReplicationCount++;

		// Update CurrentClipSize for Server, it is replicated
		CurrentClipSize--;
		OnRep_ClipSize();
		
	}

	// This variable is set for both clients and server, thus even if a server is a player, he will have the appropiate LastFireTime
	LastFireTime = GetWorld()->TimeSeconds;
}

void ASRifle::PlayFireEffect(FVector TracerEndPoint)
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

void ASRifle::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
		// SurfaceType1 is FleshDefault
		// If case is SufaceType1, will fall through and execute SurfaceType2 code which is good
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;
		// This case for headshot
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = DefaultImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

// Replicate this code when struct HitScanTrace is updated
void ASRifle::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	// Use condition b/c do not want to replicate it to client who owns this weapon
	// do not want to play visual effects twice
	DOREPLIFETIME_CONDITION(ASRifle, HitScanTrace, COND_SkipOwner);
}
