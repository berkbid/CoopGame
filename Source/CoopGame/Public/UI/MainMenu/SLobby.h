// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLobby.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USLobby : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	bool Initialize() override;

private:

	UFUNCTION()
	void OnLeaveclicked();

	UFUNCTION()
	void OnStartClicked();

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* LeaveButton;

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* StartButton;
};
