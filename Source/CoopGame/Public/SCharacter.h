// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPlayerController.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"



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

	// Allows client to run functionality when weapon is changed
	UFUNCTION()
	virtual void OnRep_CurrentWeapon();

	// Replicate this variable for owning client to have access as well
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon, BlueprintReadOnly, Category = "Player")
	class ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USWidgetCompHealthBar* HealthBar;

	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	// We replicate this boolean so when player dies on server, all clients get updated thus playing replicated death animation
	UPROPERTY(ReplicatedUsing=OnRep_Death, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnRep_Death();

	UFUNCTION(Server, Reliable)
	void ServerSetWidgetName();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	// Helper function that handles updating widget component with PlayerName from PlayerState
	void SetWidgetName();

	bool bIsInventoryFullTemp;

	int32 CurrentWeaponSlot;

public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	* Called when this Pawn is possessed. Only called on the server (or in standalone).
	* @param NewController The controller possessing this pawn
	*/
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PickupWeapon(const FWeaponInfo& WeaponInfo, AActor* PickupActor);

	/* Equips the new weapon and returns ammo count of last weapon */
	int32 EquipWeaponClass(FWeaponInfo NewWeaponInfo, int32 NewWeaponSlot);

	/** PlayerState Replication Notification Callback */
	virtual void OnRep_PlayerState() override;

};
