// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInventoryInfo.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"

void USUserWidgetInventoryInfo::AddInventoryWidget(UVerticalBox* InventoryWidget)
{
	if (InventoryBorder && InventoryWidget)
	{
		InventoryBorder->AddChild(InventoryWidget);
	}
}