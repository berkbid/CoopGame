// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASWeapon;


UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Replicate this variable for clients to have access as well!
	UPROPERTY(Replicated)
	class ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USWidgetCompHealthBar* HealthBar;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	// We replicate this boolean so when player dies on server, all clients get updated thus playing replicated death animation
	UPROPERTY(ReplicatedUsing=OnRep_Death, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnRep_Death();

public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PickupWeapon(TSubclassOf<ASWeapon> NewWeaponClass, AActor* PickupActor);

	void ChangeWeapons(TSubclassOf<ASWeapon> NewWeaponClass, int NewWeaponSlot);

};
