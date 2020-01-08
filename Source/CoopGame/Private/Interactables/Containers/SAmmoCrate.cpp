// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoCrate.h"

ASAmmoCrate::ASAmmoCrate()
{

}

void ASAmmoCrate::Interact(class APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);
}

void ASAmmoCrate::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
}


void ASAmmoCrate::OnRep_OpenContainer()
{
	Super::OnRep_OpenContainer();

}