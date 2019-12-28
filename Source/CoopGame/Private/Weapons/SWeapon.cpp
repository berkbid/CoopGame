// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";

	// Setup weapon stats
	BaseDamage = 20.f;
	HeadShotMultiplier = 4.f;
	MaxClipSize = 30;
	CurrentClipSize = MaxClipSize;

	// bullets per minute
	RateOfFire = 600.f;

	TimeBetweenShots = 60.f / RateOfFire;

	// Allows for "Server-Owned" weapons
	// Will now spawn on clients if spawned on server
	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void ASWeapon::Fire()
{
}

// Client can update HUD here when clip size changes
void ASWeapon::OnRep_ClipSize()
{
	//UE_LOG(LogTemp, Warning, TEXT("New Clip Size: %d"), CurrentClipSize);
	ASPlayerController* PC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->SetSlotAmmo(CurrentClipSize);
	}
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