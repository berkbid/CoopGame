#pragma once

#include "SWeapon.h"
#include "SweaponTypes.generated.h"


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	MiniAmmo,
	MediumAmmo,
	HeavyAmmo,
	ShellAmmo,
	RocketAmmo
};

UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
};

USTRUCT(BlueprintType)
struct FAmmoInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MiniCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MediumCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 HeavyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ShellCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 RocketCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxMiniAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxMediumAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxHeavyAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxShellAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxRocketAmmo;

	/* Helper function for retrieving ammo of a certain type, passes back how much ammo could be returned and new total for ammo type*/
	void RequestAmmo(EAmmoType AmmoTypeNeeded, int32 AmmoAmountRequested, int32& AmmoReturned, int32& NewAmmoTotal)
	{
		int32 ExtraAmmoTemp = 0;
		int32 AmmoReturnAmount = 0;

		switch (AmmoTypeNeeded)
		{
		case EAmmoType::MiniAmmo:
			ExtraAmmoTemp = MiniCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				MiniCount -= AmmoReturnAmount;
				ExtraAmmoTemp = MiniCount;
			}
			break;
		case EAmmoType::MediumAmmo:
			ExtraAmmoTemp = MediumCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				MediumCount -= AmmoReturnAmount;
				ExtraAmmoTemp = MediumCount;
			}
			break;
		case EAmmoType::HeavyAmmo:
			ExtraAmmoTemp = HeavyCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				HeavyCount -= AmmoReturnAmount;
				ExtraAmmoTemp = HeavyCount;
			}
			break;
		case EAmmoType::ShellAmmo:
			ExtraAmmoTemp = ShellCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				ShellCount -= AmmoReturnAmount;
				ExtraAmmoTemp = ShellCount;
			}
			break;
		case EAmmoType::RocketAmmo:
			ExtraAmmoTemp = RocketCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				RocketCount -= AmmoReturnAmount;
				ExtraAmmoTemp = RocketCount;
			}
			break;
		default:
			break;
		}
		AmmoReturned = AmmoReturnAmount;
		NewAmmoTotal = ExtraAmmoTemp;
	}

	/* Function to add ammo, returns left over ammo if full of specified type, and new ammo total of specified type */
	void AddAmmo(EAmmoType NewAmmoType, int32 AmmoAmount, int32& LeftOverAmmo, int32& NewAmmoTotal)
	{
		switch (NewAmmoType)
		{
		case EAmmoType::MiniAmmo:
			if (MiniCount + AmmoAmount > MaxMiniAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxMiniAmmo - MiniCount);
				MiniCount = MaxMiniAmmo;
				NewAmmoTotal = MiniCount;
			}
			else
			{
				LeftOverAmmo = 0;
				MiniCount += AmmoAmount;
				NewAmmoTotal = MiniCount;
			}
			break;
		case EAmmoType::MediumAmmo:
			if (MediumCount + AmmoAmount > MaxMediumAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxMediumAmmo - MediumCount);
				MediumCount = MaxMediumAmmo;
				NewAmmoTotal = MediumCount;
			}
			else
			{
				LeftOverAmmo = 0;
				MediumCount += AmmoAmount;
				NewAmmoTotal = MediumCount;
			}
			break;
		case EAmmoType::HeavyAmmo:
			if (HeavyCount + AmmoAmount > MaxHeavyAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxHeavyAmmo - HeavyCount);
				HeavyCount = MaxHeavyAmmo;
				NewAmmoTotal = HeavyCount;
			}
			else
			{
				LeftOverAmmo = 0;
				HeavyCount += AmmoAmount;
				NewAmmoTotal = HeavyCount;
			}
			break;
		case EAmmoType::ShellAmmo:
			if (ShellCount + AmmoAmount > MaxShellAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxShellAmmo - ShellCount);
				ShellCount = MaxShellAmmo;
				NewAmmoTotal = ShellCount;
			}
			else
			{
				LeftOverAmmo = 0;
				ShellCount += AmmoAmount;
				NewAmmoTotal = ShellCount;
			}
			break;
		case EAmmoType::RocketAmmo:
			if (RocketCount + AmmoAmount > MaxRocketAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxRocketAmmo - RocketCount);
				RocketCount = MaxRocketAmmo;
				NewAmmoTotal = RocketCount;
			}
			else
			{
				LeftOverAmmo = 0;
				RocketCount += AmmoAmount;
				NewAmmoTotal = RocketCount;
			}
			break;
		default:
			break;
		}
	}

	// For Garbage Cleanup
	void Destroy()
	{
	}

	// Default constructor
	FAmmoInfo()
	{
		MiniCount = 0;
		MediumCount = 0;
		HeavyCount = 0;
		ShellCount = 0;
		RocketCount = 0;

		MaxMiniAmmo = 500;
		MaxMediumAmmo = 500;
		MaxHeavyAmmo = 500;
		MaxShellAmmo = 500;
		MaxRocketAmmo = 500;
	}

	FAmmoInfo(int32 NewMiniAmmo, int32 NewMediumAmmo, int32 NewHeavyAmmo, int32 NewShellAmmo, int32 NewRocketAmmo)
	{
		MiniCount = NewMiniAmmo;
		MediumCount = NewMediumAmmo;
		HeavyCount = NewHeavyAmmo;
		ShellCount = NewShellAmmo;
		RocketCount = NewRocketAmmo;

		MaxMiniAmmo = 500;
		MaxMediumAmmo = 500;
		MaxHeavyAmmo = 500;
		MaxShellAmmo = 500;
		MaxRocketAmmo = 500;
	}
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ASWeapon> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponRarity WeaponRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	// For Garbage Cleanup
	void Destroy()
	{
		WeaponType = NULL;
	}

	// Default constructor
	FWeaponInfo()
	{
		WeaponType = NULL;
		WeaponName = FName();
		WeaponRarity = EWeaponRarity::Common;
		AmmoType = EAmmoType::MiniAmmo;
		CurrentAmmo = 0;
		MaxAmmo = 0;
	}

	// Constructor with parameters for properties
	FWeaponInfo(UClass* NewWeaponClass, FName NewWeaponName, EWeaponRarity NewWeaponRarity, EAmmoType NewAmmoType, int32 NewCurrentAmmo, int32 NewMaxAmmo)
	{
		WeaponType = NewWeaponClass;
		WeaponName = NewWeaponName;
		WeaponRarity = NewWeaponRarity;
		AmmoType = NewAmmoType;
		CurrentAmmo = NewCurrentAmmo;
		MaxAmmo = NewMaxAmmo;
	}
};