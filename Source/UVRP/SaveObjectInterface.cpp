// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "SaveObjectInterface.h"

USaveObjectInterface::USaveObjectInterface(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

int32 ISaveObjectInterface::GetNumber()
{
	return 0;
}

bool ISaveObjectInterface::IsExists()
{
	return false;
}

bool ISaveObjectInterface::IsExpansionStarted()
{
	return false;
}

bool ISaveObjectInterface::IsExpansionFinished()
{
	return false;
}

void ISaveObjectInterface::SetExpansionStarted()
{
}

void ISaveObjectInterface::SetExpansionFinished()
{
}

bool ISaveObjectInterface::IsActive()
{
	return false;
}

void ISaveObjectInterface::SetActive()
{
}

FTransform ISaveObjectInterface::GetTransform()
{
	return FTransform();
}

void ISaveObjectInterface::SetTransform(FTransform NewTransform)
{
}

void ISaveObjectInterface::DestroyActor()
{
}