// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SaveObjectInterface.generated.h"
/**
 * 
 */

UINTERFACE(Blueprintable)
class USaveObjectInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ISaveObjectInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	/*UFUNCTION(BlueprintNativeEvent, Category = "MenuInteractionInterface")*/

	UFUNCTION()
	virtual int32 GetNumber();

	UFUNCTION()
	virtual bool IsExists();

	UFUNCTION()
	virtual bool IsExpansionStarted();

	UFUNCTION()
	virtual bool IsExpansionFinished();

	UFUNCTION()
	virtual void SetExpansionStarted();

	UFUNCTION()
	virtual void SetExpansionFinished();

	UFUNCTION()
	virtual bool IsActive();

	UFUNCTION()
	virtual void SetActive();

	UFUNCTION()
	virtual FTransform GetTransform();

	UFUNCTION()
	virtual void SetTransform(FTransform NewTransform);

	UFUNCTION()
	virtual void DestroyActor();
};