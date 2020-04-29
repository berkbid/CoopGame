// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UUWMainMenu : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	bool Initialize() override;


private:
	UFUNCTION()
	void OnCreateLobbyClicked();

	UFUNCTION()
	void OnFindLobbyClicked();

	UFUNCTION()
	void OnJoinLobbyCancel();

	UFUNCTION()
	void OnExitClicked();

	UFUNCTION()
	void OnLobbyCreateCancel();

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenuStub;

	UPROPERTY(meta = (BindWidget))
	class ULobbyCreate* LobbyCreateStub;

	UPROPERTY(meta = (BindWidget))
	class ULobbyFind* LobbyFindStub;

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* CreateLobbyButton;

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* FindLobbyButton;

	UPROPERTY(meta = (BindWidget))
	class USButtonCustom* ExitButton;
};
