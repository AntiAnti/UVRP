// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "MenuInteractionInterface.h"


//////////////////////////////////////////////////////////////////////////
// ToStringInterface

UMenuInteractionInterface::UMenuInteractionInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool IMenuInteractionInterface::OnClick(FVector HitPoint)
{
	unimplemented();
	return true;
}

bool IMenuInteractionInterface::IsUsingCustomSound()
{
	//unimplemented();
	return false;
}
