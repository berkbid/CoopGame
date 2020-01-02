// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


enum class EAmmoType : uint8;
enum class EWeaponRarity : uint8;

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

	uint8 MaterialIndexToChange;

	class UMaterialInstanceDynamic* MatInst;

	UPROPERTY(Replicated)
	EWeaponRarity WeaponRarity;

	virtual void Fire();

	// This will only run on server instead of client, and reliable so will eventually get to server, need reliable since gameplay critical component
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	void InitializeVariables();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	/* RPM - Bullets per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// Derived from RateOfFire
	float TimeBetweenShots;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class USoundBase* WeaponSwapSound;

	int32 MaxClipSize;

	UPROPERTY(Replicated)
	int32 CurrentClipSize;

	void SetInitialState(EWeaponRarity NewWeaponRarity, int32 CurrentAmmo, int32 MaxAmmo);

	int32 GetCurrentAmmo();

	void StartFire();

	void StopFire();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
