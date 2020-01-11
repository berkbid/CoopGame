// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"
#include "SUserWidgetPlayerStats.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Texture2D.h"
#include "SWeapon.h"
#include "WidgetInventoryHUD.h"

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

// When player picks up a weapon, update inventory with weapon picture and Slot Ammo amount
void USUserWidgetGameInfo::HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo)
{
	// Find texture associated with weapon class we picked up
	UTexture2D** TempWeaponTexture = WeaponToTextureMap.Find(NewWeaponInfo.WeaponClass);

	if (InventoryHUD && TempWeaponTexture)
	{
		// Want to see if we picked up weapon that was in our currently active slot
		InventoryHUD->HandlePickupWeapon(WeaponSlot, NewWeaponInfo, *TempWeaponTexture);
	}
}

void USUserWidgetGameInfo::HandlePickupAmmo(EAmmoType NewAmmoType, int32 ExtraAmmo)
{
	if (InventoryHUD)
	{
		InventoryHUD->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}
}

// When player reloads, update all ammo text and other weapon slots sharing same ammo type needs updating
void USUserWidgetGameInfo::HandleReloadAmmoType(EAmmoType NewAmmoType, int32 CurrentAmmo, int32 ExtraAmmo)
{
	if (InventoryHUD)
	{
		// Update current slot ammo now takes care of updating current ammo for weapon info call above
		InventoryHUD->UpdateCurrentSlotAmmo(CurrentAmmo);
		InventoryHUD->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}
}

// When player shoots and current clip ammo changes, send data to relevant HUD objects
void USUserWidgetGameInfo::UpdateCurrentClipAmmo(int32 NewCurrentAmmo)
{
	if (InventoryHUD)
	{
		// Auto does currentweaponinfo call bove in new method
		InventoryHUD->UpdateCurrentSlotAmmo(NewCurrentAmmo);
	}
}

// When player changes inventory slots, handle container visual and current weapon info
void USUserWidgetGameInfo::InventoryChangeToSlot(int32 WeaponSlot)
{
	if (InventoryHUD)
	{
		InventoryHUD->HandleSlotChange(WeaponSlot);
	}
}

void USUserWidgetGameInfo::InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo)
{
	if (InventoryHUD)
	{
		InventoryHUD->InitAmmoInventory(StartingAmmoInfo);
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