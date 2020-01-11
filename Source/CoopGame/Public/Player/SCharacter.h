// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"



UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Helper function that handles updating widget component with PlayerName from PlayerState
	void SetWidgetName();

	// Allows client to run functionality when weapon is changed
	UFUNCTION()
	virtual void OnRep_CurrentWeapon();

	UFUNCTION()
	void OnRep_Death();

	UFUNCTION(Server, Reliable)
	void ServerSetWidgetName();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(Server, Reliable)
	void ServerStartFire();

	UFUNCTION(Server, Reliable)
	void ServerStopFire();

protected:
	// Replicate this variable for owning client to have access as well
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon, BlueprintReadOnly, Category = "Player")
	class ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USWidgetCompHealthBar* HealthBar;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	// We replicate this boolean so when player dies on server, all clients get updated thus playing replicated death animation
	UPROPERTY(ReplicatedUsing=OnRep_Death, BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	/* Equips the new weapon and returns ammo count of last weapon */
	void EquipWeaponClass(TSubclassOf<class ASWeapon> NewWeaponClass, const FWeaponStats& NewWeaponStats, int32 NewWeaponClipSize);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Reload();

	virtual void PossessedBy(AController* NewController) override;

	/** PlayerState Replication Notification Callback */
	virtual void OnRep_PlayerState() override;
};
