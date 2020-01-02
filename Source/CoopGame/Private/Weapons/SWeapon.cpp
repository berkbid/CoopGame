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

	// Setup weapon stats
	RateOfFire = 600.f;
	BaseDamage = 20.f;
	HeadShotMultiplier = 4.f;
	CurrentClipSize = -1;
	MaxClipSize = -1;

	MaterialIndexToChange = 0;

	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.0f;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60.f / RateOfFire;

	// Only run this code here for clients, listen server runs it elsewhere
	if (GetLocalRole() < ROLE_Authority)
	{
		InitializeVariables();
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

void ASWeapon::Fire() {}

// Server is setting these variables
void ASWeapon::SetInitialState(EWeaponRarity NewWeaponRarity, int32 CurrentAmmo, int32 MaxAmmo)
{
	// This is replicated to owner for use when firing
	CurrentClipSize = FMath::Clamp(CurrentAmmo, 0, MaxAmmo);

	MaxClipSize = MaxAmmo;

	WeaponRarity = NewWeaponRarity;

	// Run this for server here since beginplay happens before this for server
	InitializeVariables();
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

void ASWeapon::InitializeVariables()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("FOUND SERVER1"));
	}
	switch (WeaponRarity)
	{
	case EWeaponRarity::Common:
		if (GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("COMMON"));
		}
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(.18f, .18f, .18f, 1.f));
		}

		break;
	case EWeaponRarity::Uncommon:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			// This variable "LastTimeDamageTaken" is set in the editor in the graph for the material
			MatInst->SetVectorParameterValue("Color", FLinearColor(0.f, .1f, 0.f, 1.f));
		}
		break;
	case EWeaponRarity::Rare:
		if (GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("FOUND SERVER 2"));
		}
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));

		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(0.f, 0.f, 1.f, 1.f));
		}
		break;
	case EWeaponRarity::Epic:
		if (GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("FOUND SERVER 3"));
		}
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(1.f, 0.f, 1.f, 1.f));
		}
		break;
	case EWeaponRarity::Legendary:
		if (GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("FOUND SERVER 4"));
		}
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(1.f, 1.f, 0.f, 1.f));
		}
		break;
	default:
		break;
	}
}
void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, CurrentClipSize, COND_OwnerOnly);

	// Replicate this to clients so they can update texture on begin play
	DOREPLIFETIME_CONDITION(ASWeapon, WeaponRarity, COND_InitialOnly);
}