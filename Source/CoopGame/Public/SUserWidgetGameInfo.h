// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetGameInfo.generated.h"

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

	//you can make the binding optional with BindWidgetOptional instead. But don�t forget to null check!
/* Reference to HealthBar ProgressBar in Widget*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* ScoreboardEntryBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* StateText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* FirstSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* SecondSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* ThirdSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* FourthSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* FifthSlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* FirstOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* SecondOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* ThirdOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* FourthOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* FifthOverlay;

	class UOverlay* CurrentOverlay;

	void ResetOldInventorySlot();

	void UpdateNewInventorySlot(class UOverlay* NewOverlay);

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
	TSubclassOf<class USUserWidgetPlayerStats> wPlayerStats;

	//class USUserWidgetPlayerStats* MyPlayerStats;

public:

	virtual bool Initialize() override;

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "LODZERO|UI")
	void SetOwningController(class APlayerController* NewController);

	UFUNCTION()
	void HandleScoreChanged(float NewScore);

	UFUNCTION()
	void UpdateInventoryHUD(int WeaponSlot);

	UFUNCTION(BlueprintImplementableEvent)
	void SetInventoryImage(TSubclassOf<class ASWeapon> InventoryItemClass, int BorderSlot);

	void AddPlayerToScoreboard(FString NewName, FString NewKills, FString NewDeath, FString NewScore);
	
};
