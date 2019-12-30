// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"
#include "SUserWidgetPlayerStats.h"
#include "SOverlayInventorySlot.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Texture2D.h"
#include "SWeapon.h"
#include "SHorizontalBoxInventory.h"
#include "SPlayerController.h"
#include "SVerticalBoxCurrentWeapon.h"

bool USUserWidgetGameInfo::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

// Player controller calls this after creating this widget and passes reference to itself
void USUserWidgetGameInfo::SetOwningController(APlayerController* NewController)
{
	// Skip repeated calls
	if (OwningController == NewController) { return; }
	OwningController = NewController;
}

// When player changes inventory slots, handle container visual and current weapon info
void USUserWidgetGameInfo::InventoryChangeToSlot(int32 WeaponSlot, int32 CurrentAmmo, int32 ExtraAmmo)
{
	// Handle inventory visual for changing slot 
	if (InventoryContainer)
	{
		InventoryContainer->HandleSlotChange(WeaponSlot);
	}
	// Handle current weapon info showing new slot info
	SetWeaponAmmo(CurrentAmmo, ExtraAmmo);
}

// Whenever we set slot ammo, we ant to set weapon ammo also
// When player reloads or shoots, this is only time SlotAmmo changes, not from swapping weapons
void USUserWidgetGameInfo::SetSlotAndWeaponAmmo(int32 WeaponSlot, int32 CurrentAmmo, int32 ExtraAmmo)
{
	if (InventoryContainer)
	{
		InventoryContainer->SetSlotAmmo(WeaponSlot, CurrentAmmo + ExtraAmmo);
	}
	SetWeaponAmmo(CurrentAmmo, ExtraAmmo);
}

// When player reloads or shoots, weapon ammo changes, also reload calls this
void USUserWidgetGameInfo::SetWeaponAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (CurrentWeaponInfo)
	{
		CurrentWeaponInfo->SetWeaponAmmo(CurrentAmmo, TotalAmmo);
	}
}

// When player picks up a weapon, update inventory with weapon picture and Slot Ammo amount
void USUserWidgetGameInfo::HandlePickupWeapon(int32 WeaponSlot, TSubclassOf<ASWeapon> InventoryItemClass, int32 SlotTotal)
{
	// Find texture associated with weapon class we picked up
	UTexture2D** TempWeaponTexture = WeaponToTextureMap.Find(InventoryItemClass);

	if (InventoryContainer)
	{
		// Check pointer before dereferencing it
		if (TempWeaponTexture)
		{
			// initializes WeaponSlot with weapon image and slot total ammo text
			InventoryContainer->HandlePickupWeapon(WeaponSlot, *TempWeaponTexture, SlotTotal);
		}

	}
}

void USUserWidgetGameInfo::AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber)
{
	if (wPlayerStats)
	{
		USUserWidgetPlayerStats* NewPlayerStats = CreateWidget<USUserWidgetPlayerStats>(this, wPlayerStats);
		if (NewPlayerStats && ScoreboardEntryBox)
		{
			NewPlayerStats->SetAllText(NewPlayerName, FString("0"), FString("0"), FString("0"));
			ScoreboardEntryBox->AddChild(NewPlayerStats);

			// Add reference to PlayerStats to ScoreboardDictionary for updating in the future
			ScoreboardDictionary.Add(NewPlayerNumber, NewPlayerStats);
		}
	}
}

void USUserWidgetGameInfo::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetScoreText(FString::SanitizeFloat(NewScore));
	}
}

void USUserWidgetGameInfo::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetKillText(FString::FromInt(NewKills));
	}
}

void USUserWidgetGameInfo::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetDeathText(FString::FromInt(NewDeaths));
	}
}

void USUserWidgetGameInfo::SetStateText(FString NewState)
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(NewState));
	}
}

void USUserWidgetGameInfo::SetMiniAmmoText(FString NewText)
{
	if (MiniAmmoText)
	{

		MiniAmmoText->SetText(FText::FromString("Mini: " + NewText));
	}
}

void USUserWidgetGameInfo::SetMediumAmmoText(FString NewText)
{
	if (MediumAmmoText)
	{
		MediumAmmoText->SetText(FText::FromString("Medium: " + NewText));
	}
}

void USUserWidgetGameInfo::SetHeavyAmmoText(FString NewText)
{
	if (HeavyAmmoText)
	{
		HeavyAmmoText->SetText(FText::FromString("Heavy: " + NewText));
	}
}

void USUserWidgetGameInfo::SetShellAmmoText(FString NewText)
{
	if (ShellAmmoText)
	{
		ShellAmmoText->SetText(FText::FromString("Shell: " + NewText));
	}
}

void USUserWidgetGameInfo::SetRocketAmmoText(FString NewText)
{
	if (RocketAmmoText)
	{
		RocketAmmoText->SetText(FText::FromString("Rocket: " + NewText));
	}
}