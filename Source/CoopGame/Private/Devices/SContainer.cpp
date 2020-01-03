// Fill out your copyright notice in the Description page of Project Settings.


#include "SContainer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CoopGame.h"
#include "SWidgetCompPickupInfo.h"

// Sets default values
ASContainer::ASContainer()
{
	BoxComp->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void ASContainer::BeginPlay()
{
	Super::BeginPlay();
	
}


//void ASContainer::ShowItemInfo()
//{
//	Super::ShowItemInfo();
//
//
//}

