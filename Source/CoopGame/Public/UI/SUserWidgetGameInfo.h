// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetGameInfo.generated.h"

class UTextBlock;
class UBorder;
class USOverlayInventorySlot;

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetGameInfo : public UUserWidget
{
	GENERATED_BODY()

protected:

	/* Actor that widget is attached to via WidgetComponent */
	UPROPERTY(BlueprintReadOnly, Category = "ActorWidget")
	APlayerController* OwningController;

	//you can make the binding optional with BindWidgetOptional instead. But don’t forget to null check!
/* Reference to HealthBar ProgressBar in Widget*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* ScoreboardEntryBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* StateText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USOverlayInventorySlot* FirstOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USOverlayInventorySlot* SecondOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USOverlayInventorySlot* ThirdOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USOverlayInventorySlot* FourthOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USOverlayInventorySlot* FifthOverlay;

	USOverlayInventorySlot* CurrentOverlay;

	void ResetOldInventorySlot();

	void UpdateNewInventorySlot(USOverlayInventorySlot* NewOverlay);

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
	TSubclassOf<class USUserWidgetPlayerStats> wPlayerStats;

	/* Mapping from unique PlayerID to their scoreboard PlayerStats object reference */
	TMap<uint32, class USUserWidgetPlayerStats*> ScoreboardDictionary;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TMap<TSubclassOf<class ASWeapon>, class UTexture2D* > WeaponToTextureMap;

public:

	virtual bool Initialize() override;

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "LODZERO|UI")
	void SetOwningController(class APlayerController* NewController);

	UFUNCTION()
	void HandleScoreChanged(float NewScore);

	UFUNCTION()
	void UpdateInventoryHUD(int WeaponSlot);

	void HandlePickupWeapon(TSubclassOf<class ASWeapon> InventoryItemClass, int WeaponSlot);

	UFUNCTION(BlueprintImplementableEvent)
	void SetInventoryImage(TSubclassOf<class ASWeapon> InventoryItemClass, int BorderSlot);

	void SetStateText(FString NewState);

	void AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber);

	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);

	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);

	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);
	
};
