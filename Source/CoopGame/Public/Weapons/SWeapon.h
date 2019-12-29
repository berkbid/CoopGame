// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class ASWeapon> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	void SetWeaponType(TSubclassOf<class ASWeapon> NewWeaponType)
	{
		WeaponType = NewWeaponType;
	}

	void SetCurrentAmmo(uint32 NewCurrentAmmo)
	{
		CurrentAmmo = NewCurrentAmmo;
	}

	void SetMaxAmmo(uint32 NewMaxAmmo)
	{
		MaxAmmo = NewMaxAmmo;
	}

	// For Garbage Cleanup
	void Destroy()
	{
		WeaponType = nullptr;
	}

	// Default constructor
	FWeaponInfo()
	{
		WeaponType = nullptr;
		CurrentAmmo = 0;
		MaxAmmo = 0;
	}

	// Constructor with parameters for properties
	FWeaponInfo(TSubclassOf<class ASWeapon> NewWeaponClass, int32 NewCurrentAmmo, int32 NewMaxAmmo)
	{
		WeaponType = NewWeaponClass;
		CurrentAmmo = NewCurrentAmmo;
		MaxAmmo = NewMaxAmmo;
	}
};



UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadShotMultiplier;

	virtual void Fire();

	// This will only run on server instead of client, and reliable so will eventually get to server, need reliable since gameplay critical component
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// Derived from RateOfFire
	float TimeBetweenShots;

	UPROPERTY(Replicated)
	int32 CurrentWeaponSlot;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	class USoundBase* WeaponSwapSound;

	int32 MaxClipSize;

	UFUNCTION()
	void OnRep_ClipSize();

	UPROPERTY(ReplicatedUsing=OnRep_ClipSize)
	int32 CurrentClipSize;

	void SetInitialState(int32 CurrentAmmo, int32 MaxAmmo, int32 WeaponSlot);

	int32 GetCurrentAmmo();

	void StartFire();

	void StopFire();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
