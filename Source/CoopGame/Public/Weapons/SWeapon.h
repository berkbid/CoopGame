// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadShotMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FWeaponStats WeaponStats;

	virtual void Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class USoundBase* WeaponSwapSound;


	UPROPERTY(Replicated)
	int32 CurrentClipSize;

	void InitWeaponState(int32 CurrentAmmo);

	int32 GetCurrentAmmo() const;

	/* Called by server from SCharacter */
	void StartFire();

	/* Called by server from SCharacter */
	void StopFire();
};
