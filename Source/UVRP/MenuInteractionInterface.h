// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MenuInteractionInterface.generated.h"

UINTERFACE(Blueprintable)
class UMenuInteractionInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IMenuInteractionInterface
{
	GENERATED_IINTERFACE_BODY()
		
public:
	/*UFUNCTION(BlueprintNativeEvent, Category = "MenuInteractionInterface")*/

	UFUNCTION()
	virtual bool OnClick(FVector HitPoint);

	UFUNCTION()
	virtual bool IsUsingCustomSound();
};