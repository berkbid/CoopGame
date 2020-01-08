// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Components/VerticalBox.h"
#include "SVerticalBoxCurrentWeapon.generated.h"


/**
 * 
 */
UCLASS()
class COOPGAME_API USVerticalBoxCurrentWeapon : public UVerticalBox
{
	GENERATED_BODY()
	


public:
	USVerticalBoxCurrentWeapon();

	/**
	 * Applies all properties to the native widget if possible.  This is called after a widget is constructed.
	 * It can also be called by the editor to update modified state, so ensure all initialization to a widgets
	 * properties are performed here, or the property and visual state may become unsynced.
	 */
	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	void InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo);

	void QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	void SetWeaponCurrentAmmo(int32 NewCurrentAmmo);

	void SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo);

	void SetWeaponExtraAmmo(int32 NewExtraAmmo);


protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UOverlay* CurrentWeaponAmmo;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UBorder* CurrentWeaponType;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AmmoTextures")
	TMap<EAmmoType, class UTexture2D*> AmmoToTextureMap;

	FWeaponInfo CurrentWeaponInfo;
	int32 ExtraClipSize;

	void SetAmmoText();

	void SetWeaponText();

	void SetAmmoType();
};