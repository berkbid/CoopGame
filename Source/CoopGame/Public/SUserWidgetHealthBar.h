// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetHealthBar : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	/* Actor that widget is attached to via WidgetComponent */
	UPROPERTY(BlueprintReadOnly, Category = "ActorWidget")
	AActor* OwningActor;

	//you can make the binding optional with BindWidgetOptional instead. But don’t forget to null check!
	/* Reference to HealthBar ProgressBar in Widget*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	float MaxHealth;

public:

	virtual bool Initialize() override;

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "LODZERO|UI")
	void SetOwningActor(AActor* NewOwner);

	void SetNameText(FString NewName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "New Widget")
	FString PlayerWidget;

	UFUNCTION()
	void HandleHealthChanged(class USHealthComponent* HealthComp, float Health, float HealthDelt, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//UFUNCTION(BlueprintImplementableEvent)
	//void UpdateHealthBar(class UMMHealthComponent* HealthComp);
};
