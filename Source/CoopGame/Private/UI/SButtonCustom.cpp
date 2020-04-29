// Fill out your copyright notice in the Description page of Project Settings.


#include "SButtonCustom.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

USButtonCustom::USButtonCustom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Adjustable variables for use to customize the button in editor on pre-construct
	NewButtonText = FText::FromString("Default");
	NewButtonTextSize = 24;
}

bool USButtonCustom::Initialize()
{
	bool bShouldInitialize = Super::Initialize();
	if (!bShouldInitialize) { return false; }

	if (ButtonWidg)
	{
		ButtonWidg->OnClicked.RemoveDynamic(this, &USButtonCustom::OnButtonWidgClicked);
		ButtonWidg->OnClicked.AddDynamic(this, &USButtonCustom::OnButtonWidgClicked);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Button Initialize"));

	return bShouldInitialize;
}

void USButtonCustom::NativePreConstruct()
{
	// The Super() calls the blueprint PreConstruct() function
	if (ButtonText)
	{
		ButtonText->SetText(NewButtonText);
		//ButtonText->Font.Size = NewButtonTextSize;
		
		FSlateFontInfo ButtonFontInfo = ButtonText->Font;
		ButtonFontInfo.Size = NewButtonTextSize;
		ButtonText->SetFont(ButtonFontInfo);
	}

}

void USButtonCustom::OnButtonWidgClicked()
{
	// Broadcast our own delegate when our button object is clicked
	OnSButtonClicked.Broadcast();
}
