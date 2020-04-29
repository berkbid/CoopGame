// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerEntry.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class COOPGAME_API UServerEntry : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void SetupServerEntry(const FString& NewServerName, const FString& NewHostName, int32 NewNumPlayers, int32 NewMaxPlayers);


protected:
	bool Initialize() override;

	
private:
	UFUNCTION()
	void OnEntryClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* EntryButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* EntryBorder;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LobbyNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IsPrivateText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayersText;
};
