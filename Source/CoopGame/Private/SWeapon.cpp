// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


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
	BaseDamage = 20.f;
	HeadShotMultiplier = 4.f;
	BulletSpread = 2.f;

	// bullets per minute
	RateOfFire = 600.f;

	MuzzleEffect = nullptr;
	DefaultImpactEffect = nullptr;
	TracerEffect = nullptr;

	// Allows for "Server-Owned" weapons
	// Will now spawn on clients if spawned on server
	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / RateOfFire;

}


// Want this to happen on server
void ASWeapon::Fire()
{
	// So if we are a client, we run ServerFire() instead of this Fire() function
	if (Role < ROLE_Authority)
	{
		ServerFire();
		// If we return here, the owning client will not run this code for itself
		//return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		// We override the location return in SCharacter.cpp to return camera location instead
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection =  FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

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
			//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			// Use Get() for weak object pointer??
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

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
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
		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			// Only overriden from default if we hit something
			HitScanTrace.SurfaceType = SurfaceType;
			HitScanTrace.ReplicationCount++;
			
			
		}

		// This variable is set for both clients and server, thus even if a server is a player, he will have the appropiate LastFireTime
		LastFireTime = GetWorld()->TimeSeconds;
	}

}

// MUST prefix with Server and require _Implementation
void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	// This is for anti cheat stuff
	return true;
}

// Replicate this code when struct HitScanTrace is updated
void ASWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic FX
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::StartFire()
{
	// This makes it so you cannot single fire faster than you can automatic fire
	// Use greatest of first or 2nd value, clamps between 0 and other value
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}


void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
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

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
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

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	// Use condition b/c do not want to replicate it to client who owns this weapon
	// do not want to play visual effects twice
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(ASWeapon, LastFireTime, COND_OwnerOnly);
}


