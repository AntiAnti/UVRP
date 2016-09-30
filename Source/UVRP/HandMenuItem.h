// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "HandMenuEmpty.h"
#include "HandMenuItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API UHandMenuItem : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	static const int ScreenHeight = 10;

public:
	UHandMenuItem();

	virtual void BeginPlay() override;

	UFUNCTION()
	void SetItem(EMenuItemName item, EMenuItemType type);

	UFUNCTION()
	EMenuItemName GetItem() { return ItemType; };

	UFUNCTION()
	void SetItemActive(bool NewState);

	UFUNCTION()
	void SetItemColor(uint8 ItemType);

private:
	UPROPERTY()
	EMenuItemName ItemType;
	UPROPERTY()
	EMenuItemType ItemTypeVal;
	UPROPERTY()
	UMaterialInstanceDynamic *mtiMenu;

	UFUNCTION()
	void GetUV(EMenuItemName item, float &u, float &v, int &texture);
};
