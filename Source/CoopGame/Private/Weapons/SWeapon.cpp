// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	WeaponStats = FWeaponStats();

	// Setup weapon stats
	HeadShotMultiplier = 4.f;
	CurrentClipSize = -1;

	SetReplicates(true);

	NetUpdateFrequency = 144.f;
	MinNetUpdateFrequency = 33.0f;
	NetPriority = 1.f;
}

// Server is setting these variables
void ASWeapon::InitWeaponState(int32 CurrentAmmo)
{
	CurrentClipSize = CurrentAmmo;
}

// Call this on server instead of client
void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + WeaponStats.FireRate - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, WeaponStats.FireRate, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Fire() {}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}