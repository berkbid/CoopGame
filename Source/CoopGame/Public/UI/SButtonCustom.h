// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SButtonCustom.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSButtonClicked);

/**
 * 
 */
UCLASS()
class COOPGAME_API USButtonCustom : public UUserWidget
{
	GENERATED_BODY()
	

public:
	USButtonCustom(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Appearance")
	FText NewButtonText;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Appearance")
	int32 NewButtonTextSize;

	/** Delegate for when button object is clicked */
	FOnSButtonClicked OnSButtonClicked;


protected:
	bool Initialize() override;

	void NativePreConstruct() override;



private:
	UFUNCTION()
	void OnButtonWidgClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonWidg;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ButtonText;

};
