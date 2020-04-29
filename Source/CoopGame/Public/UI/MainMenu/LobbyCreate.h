// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyCreate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelCreate);

/**
 * 
 */
UCLASS()
class COOPGAME_API ULobbyCreate : public UUserWidget
{
	GENERATED_BODY()
	

public:
	/** Delegate for when button object is clicked */
	FOnCancelCreate OnCancelCreate;

protected:
	bool Initialize() override;

private:

	UFUNCTION()
	void OnCancelclicked();

	UFUNCTION()
	void OnCreateClicked();

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* Createbutton;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* LobbyName;
};
