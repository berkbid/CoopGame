// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";

	// Setup weapon stats
	RateOfFire = 600.f;
	BaseDamage = 20.f;
	HeadShotMultiplier = 4.f;
	CurrentClipSize = -1;
	MaxClipSize = -1;

	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / RateOfFire;
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

void ASWeapon::Fire() {}

// Server is setting these variables
void ASWeapon::SetInitialState(int32 CurrentAmmo, int32 MaxAmmo)
{
	// These are replicated to owner for use when firing
	CurrentClipSize = FMath::Clamp(CurrentAmmo, 0, MaxAmmo);

	// These are not replicated
	MaxClipSize = MaxAmmo;
}

int32 ASWeapon::GetCurrentAmmo()
{
	return CurrentClipSize;
}

// Call this on server instead of client
void ASWeapon::StartFire()
{
	// This doesn't fix weapon swapping to shoot faster because LastFireTime gets reset when weapon is reconstructed
	// This makes it so you cannot single fire faster than you can automatic fire
	// Use greatest of first or 2nd value, clamps between 0 and other value
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, CurrentClipSize, COND_OwnerOnly);
}