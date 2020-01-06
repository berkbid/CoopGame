// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SUserWidgetInventoryInfo.h"
#include "SPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "SWeapon.h"
#include "CoopGame.h"
#include "DrawDebugHelpers.h"
#include "SInteractable.h"
#include "BluePrint/WidgetBlueprintLibrary.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentSlot = 0;
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;

	// Setup initial WeaponInventory with appropriate size and NULL values
	WeaponInventory.Init(FWeaponInfo(), InventoryMaxSize);
	AmmoInventory = FAmmoInfo(0, 0, 0, 0, 0, 250, 250, 250, 250, 250);

}

void ASPlayerController::PostInitProperties()
{
	Super::PostInitProperties();

	// Count current inventory size
	for (int32 i = 0; i != WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i].WeaponType)
		{
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}
		}
	}
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Update some HUD info manually here for clients since they might load in late
	if (MyGameInfo)
	{
		ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
		if (GS)
		{
			MyGameInfo->SetStateText(GS->GetWaveStateString());
		}
	}
}

// This gets replicated when PlayerState is assigned to this controller and is valid for the first time
// GameState isn't always valid at this point
void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}

/** spawns and initializes the PlayerState for this Controller */
// Called before possess and postlogin, only server runs this code
void ASPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

// Only PlayerState for server is valid in GameState
void ASPlayerController::ServerPostLogin()
{
	// Initialize HUD on client side
	ClientPostLogin();

	// Loop through WeaponInventory array and update HUD images if weapons are present
	int32 WeaponInventoryLen = WeaponInventory.Num();
	// Loop through our weapon inventory and populate HUD with the data
	for (int32 i = 0; i != WeaponInventoryLen; ++i)
	{
		// Handle HUD picking up new weapon
		ClientPickupWeaponHUD(WeaponInventory[i], CurrentSlot, i);

		// Handle HUD changing to slot
		if (i == CurrentSlot)
		{
			ClientChangeToSlotHUD(i);
		}
	}

	// Initialize ammo info for HUD
	ClientInitAmmoInventoryHUD(AmmoInventory);
}

// Playerstate and gamestate not valid for clients here
void ASPlayerController::ClientPostLogin_Implementation()
{
	// Create and setup initial HUD state
	// Only local controllers can add widgets
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (!wGameInfo) { return; }
		MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);
		if (!MyGameInfo) { return; }

		// Pass reference of ourself to widget while calling setup logic on widget
		MyGameInfo->SetOwningController(this);
		MyGameInfo->AddToViewport();
	}
}

void ASPlayerController::ClientAddPlayerToHUDScoreboard_Implementation(FString const &NewPlayerName, uint32 NewPlayerNumber)
{
	if (MyGameInfo)
	{
		MyGameInfo->AddPlayerToScoreboard(NewPlayerName, NewPlayerNumber);
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//check(InputComponent);
	// PlayerController is in charge of weapon changing, this can happen without a pawn, pawn uses the weapons
	InputComponent->BindAction("Weapon1", IE_Pressed, this, &ASPlayerController::EquipSlotOne);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &ASPlayerController::EquipSlotTwo);
	InputComponent->BindAction("Weapon3", IE_Pressed, this, &ASPlayerController::EquipSlotThree);
	InputComponent->BindAction("Weapon4", IE_Pressed, this, &ASPlayerController::EquipSlotFour);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);
	InputComponent->BindAction("Interact", IE_Pressed, this, &ASPlayerController::Interact);
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ASPlayerController::ToggleInventory);
}

void ASPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// PlayerState is valid at this point, but playername is not set, so PostLogin hasent been called
	if (GetLocalRole() == ROLE_Authority)
	{
		// Uncomment to have players reset to current slot being 0 upon possessing new pawn
		//CurrentSlot = 0;
		//OnRep_SlotChange();

		// If we possess a SPlayerCharacter, equip weapon at CurrentSlot index
		ASPlayerCharacter* MySPlayerChar = Cast<ASPlayerCharacter>(GetPawn());
		if (MySPlayerChar)
		{
			// If our current slot is a valid inventory index, have player equip contents of inventory index
			int32 WeaponInventoryLen = WeaponInventory.Num();
			if (WeaponInventoryLen > CurrentSlot)
			{
				// Equip whatever weapon is in the current slot if we are a SPlayerCharacter
				MySPlayerChar->EquipWeaponClass(WeaponInventory[CurrentSlot]);
			}
		}
	}
}

// Method that handles equipping a new weapon slot of the inventory, only server should ever call this function
void ASPlayerController::EquipWeapon(int NewWeaponSlot)
{
	// Only server should call this function, this is precautionary
	if (GetLocalRole() < ROLE_Authority) { return; }

	// Equip new weapon slot even if it is empty and update HUD
	if (WeaponInventory.Num() > NewWeaponSlot)
	{
		// Call ChangeWeapons, allowing for NewWeaponClass to be null going in, this will destroy current weapon
		// It is the same as changing to a new inventory slot that doesn't have a weapon in it
		ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->EquipWeaponClass(WeaponInventory[NewWeaponSlot]);
		}
		// Change active slot even if no pawn is possessed, do this after we SetCurrentSlotAmmo above
		CurrentSlot = NewWeaponSlot;
		ClientChangeToSlotHUD(CurrentSlot);
	}
}

// Try to pick up weapon if inventory has space, return success or failure
bool ASPlayerController::PickedUpNewWeapon(const FWeaponInfo& WeaponInfo)
{
	if (GetLocalRole() < ROLE_Authority) { return false; }
	if (bIsInventoryFull) { return false; }

	// Loop through inventory looking for empty slot
	int32 InventorySize = WeaponInventory.Num();
	for (int32 i = 0; i < InventorySize; ++i)
	{
		// If we find an empty slot, give new weapon type and update HUD image for slot
		if (!WeaponInventory[i].WeaponType)
		{
			// Update weapon inventory slot to new weaponclass
			WeaponInventory[i] = WeaponInfo;

			// Update HUD elements for new weapon, also pass extra ammo if this weapon's ammo type
			ClientPickupWeaponHUD(WeaponInfo, CurrentSlot, i);

			// Update inventory size variable and set bIsInventoryFull
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}

			// If we pick up a weapon into our current selected EMPTY slot, equip the weapon!
			if (CurrentSlot == i)
			{
				ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
				if (MyPawn)
				{
					// This returns ammo amount of old weapon but we don't need it in this case
					MyPawn->EquipWeaponClass(WeaponInfo);
				}
			}
			else
			{
				// Only want to play pickup sound in this scenario, or don't want to play weapon swap sound in "if" clause above
			}
			//We officially equipped new weapon, return success
			return true;
		}
	}
	// Return failure because we didn't find empty slot for weapon
	return false;
}

int32 ASPlayerController::PickedUpNewAmmo(EAmmoType AmmoType, int32 AmmoAmount)
{
	int32 NewReturnedAmmo = 0;
	int32 NewExtraAmmo = 0;

	AmmoInventory.AddAmmo(AmmoType, AmmoAmount, NewReturnedAmmo, NewExtraAmmo);

	// If we grabbed SOME ammo, update HUD
	if (NewReturnedAmmo < AmmoAmount)
	{
		ClientPickupAmmoHUD(AmmoType, NewExtraAmmo);
	}
	
	return NewReturnedAmmo;
}

int32 ASPlayerController::ReloadAmmoClip(int32 CurrentClipSize)
{
	int32 AmmmoNeeded = WeaponInventory[CurrentSlot].WeaponStats.MagazineSize - CurrentClipSize;
	if (AmmmoNeeded <= 0) { return 0; }

	// Retrieve ammo from ammo inventory
	int32 AmmoReturnAmount = 0;
	int32 ExtraAmmoTemp = 0;
	EAmmoType AmmoTypeNeeded = WeaponInventory[CurrentSlot].AmmoType;
	AmmoInventory.RequestAmmo(AmmoTypeNeeded, AmmmoNeeded, AmmoReturnAmount, ExtraAmmoTemp);

	// Update HUD with new ammo inventory change
	ClientHandleReloadHUD(AmmoTypeNeeded, CurrentClipSize + AmmoReturnAmount, ExtraAmmoTemp);

	return AmmoReturnAmount;
}

void ASPlayerController::UpdateCurrentClip(int32 NewClipSize)
{
	// Update current ammo of current slot and update HUD
	WeaponInventory[CurrentSlot].CurrentAmmo = NewClipSize;
	ClientUpdateClipHUD(NewClipSize);
}

void ASPlayerController::SetStateTextHUD(FString NewState)
{
	if (MyGameInfo)
	{
		MyGameInfo->SetStateText(NewState);
	}
}

void ASPlayerController::UpdatePlayerScoreHUD(uint32 PlayerNumber, float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerScore(PlayerNumber, NewScore);
	}
}

void ASPlayerController::UpdatePlayerKillsHUD(uint32 PlayerNumber, uint32 NewKills)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerKills(PlayerNumber, NewKills);
	}
}

void ASPlayerController::UpdatePlayerDeathsHUD(uint32 PlayerNumber, uint32 NewDeaths)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerDeaths(PlayerNumber, NewDeaths);
	}
}

void ASPlayerController::Interact()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerInteract();
		return;
	}

	// Perform a line trace that mimics SPlayerCharacter's line trace on Tick()
	FVector EyeLocation;
	FRotator EyeRotation;
	// We override the location return in SCharacter.cpp to return camera location instead
	GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector TraceDirection = EyeRotation.Vector();
	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + (TraceDirection * 450.f);
	//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Yellow, false, 2.f, 0, 5.f);

	FCollisionObjectQueryParams ObjectParams; 
	ObjectParams.AddObjectTypesToQuery(COLLISION_INTERACTABLEOBJECT);

	TArray<FHitResult> HitArray;
	bool bDidWeHit = GetWorld()->LineTraceMultiByObjectType(HitArray, TraceStart, TraceEnd, ObjectParams);

	if (bDidWeHit)
	{
		ASInteractable* TempInteractable = nullptr;

		for (FHitResult HR : HitArray)
		{
			AActor* HitActor = HR.GetActor();
			if (!HitActor) { continue; }
			ASInteractable* HitInteractable = Cast<ASInteractable>(HitActor);
			if (!HitInteractable) { continue; }

			// Keep pointer to latest hit interactable in HitArray
			TempInteractable = HitInteractable;
		}
		// Interact with the interactable and it will call functions like PickedUpNewWeapon or PickedUpNewAmmo on this class
		if (TempInteractable) 
		{
			TempInteractable->Interact(this);
		}
	}
}

void ASPlayerController::ToggleInventory()
{
	// If we don't have MyInventoryInfo object, try to create and display it
	if (!MyInventoryInfo)
	{
		if (!wInventoryInfo) { return; }
		MyInventoryInfo = CreateWidget<USUserWidgetInventoryInfo>(this, wInventoryInfo);
		if (!MyInventoryInfo) { return; }

		if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::Hidden); }


		MyInventoryInfo->AddToViewport();
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyInventoryInfo);
		bShowMouseCursor = true;
	}
	// If we already have a MyInventoryInfo object, determine if it is in viewport and toggle off or on
	else
	{
		if (MyInventoryInfo->IsInViewport())
		{
			if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::HitTestInvisible); }

			MyInventoryInfo->RemoveFromViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
			bShowMouseCursor = false;
		}
		else
		{
			if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::Hidden); }
			MyInventoryInfo->AddToViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyInventoryInfo);
			bShowMouseCursor = true;
		}
	}
}

void ASPlayerController::EquipSlotOne()
{
	ServerEquipWeaponOne();
}

void ASPlayerController::EquipSlotTwo()
{
	ServerEquipWeaponTwo();
}

void ASPlayerController::EquipSlotThree()
{
	ServerEquipWeaponThree();
}

void ASPlayerController::EquipSlotFour()
{
	ServerEquipWeaponFour();
}

void ASPlayerController::EquipSlotFive()
{
	ServerEquipWeaponFive();
}

void ASPlayerController::ServerInteract_Implementation()
{
	Interact();
}

void ASPlayerController::ServerEquipWeaponOne_Implementation()
{
	if (CurrentSlot == 0) { return; }
	EquipWeapon(0);
}

void ASPlayerController::ServerEquipWeaponTwo_Implementation()
{
	if (CurrentSlot == 1) { return; }
	EquipWeapon(1);
}

void ASPlayerController::ServerEquipWeaponThree_Implementation()
{
	if (CurrentSlot == 2) { return; }
	EquipWeapon(2);
}

void ASPlayerController::ServerEquipWeaponFour_Implementation()
{
	if (CurrentSlot == 3) { return; }
	EquipWeapon(3);
}

void ASPlayerController::ServerEquipWeaponFive_Implementation()
{
	if (CurrentSlot == 4) { return; }
	EquipWeapon(4);
}

void ASPlayerController::ClientInitAmmoInventoryHUD_Implementation(const FAmmoInfo& NewAmmoInfo)
{
	if (MyGameInfo)
	{
		MyGameInfo->InitAmmoInventory(NewAmmoInfo);
	}
}

void ASPlayerController::ClientChangeToSlotHUD_Implementation(int32 NewSlot)
{
	if (!MyGameInfo) { return; }

	// Call HUD method to change slot with ammo info
	MyGameInfo->InventoryChangeToSlot(NewSlot);
}

void ASPlayerController::ClientHandleReloadHUD_Implementation(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo)
{
	if (!MyGameInfo) { return; }

	MyGameInfo->HandleReloadAmmoType(NewAmmoType, NewClipAmmo, NewExtraAmmo);

}

// This update for HUD is equivalent to equipping and un-equipping the weapon, show weapon image or remove it
void ASPlayerController::ClientPickupWeaponHUD_Implementation(const FWeaponInfo& WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate)
{
	// Handle HUD for picking up new weapon
	if (!MyGameInfo) { return; }

	// THis uses weaponinfo for current ammo which is wrong
	// Set initial HUD state for weapon slot, including picture and ammo amount
	MyGameInfo->HandlePickupWeapon(SlotToUpdate, WeaponInfo);

	// Only play weapon pickup sound if we aren't already selecting on new weapon slot, this is because weapon swap sound will play
	if (TempCurrentSlot != SlotToUpdate)
	{
		// Play pickup sound also
		if (PickupWeaponSound)
		{
			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PickupWeaponSound, ControlledPawn->GetActorLocation());
			}
		}
	}
}

void ASPlayerController::ClientPickupAmmoHUD_Implementation(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	if (MyGameInfo)
	{
		MyGameInfo->HandlePickupAmmo(NewAmmoType, NewExtraAmmo);
	}

	// Play pickup sound also
	if (PickupAmmoSound)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupAmmoSound, ControlledPawn->GetActorLocation());
		}
	}
}

void ASPlayerController::ClientUpdateClipHUD_Implementation(int32 CurrentAmmo)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdateCurrentClipAmmo(CurrentAmmo);
	}
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}