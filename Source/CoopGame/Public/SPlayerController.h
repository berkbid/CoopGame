// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	
	ASPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	void SetScoreText(float NewScore);

	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameInfo;

	class USUserWidgetGameInfo* MyGameInfo;

	void PickedUpNewWeapon(TSubclassOf<class ASWeapon> WeaponClass);

protected:
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipWeapon(int NewWeaponSlot);

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<class ASWeapon>> WeaponInventory;

	UFUNCTION()
	void OnRep_SlotChange();

	/* Keep track of which weapon slot is currently equipped */
	UPROPERTY(ReplicatedUsing=OnRep_SlotChange)
	int CurrentSlot;

	UFUNCTION()
	void OnRep_SlotToUpdate();

	/* This is so client can update HUD slot image when it needs to be changed */
	UPROPERTY(ReplicatedUsing = OnRep_SlotToUpdate)
	int SlotToUpdate;

};
