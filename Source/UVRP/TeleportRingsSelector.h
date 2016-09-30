// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "TeleportRingsSelector.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UVRP_API UTeleportRingsSelector : public UStaticMeshComponent
{
	GENERATED_BODY()

	UTeleportRingsSelector();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	USkeletalMeshComponent *CurrentTeleportMesh;
	UPROPERTY()
	FVector RelativeLocationToSocket;
	UPROPERTY()
	int32 CurrentRing;
	UPROPERTY()
	bool SelectorValue;
	UPROPERTY()
	UMaterialInstanceDynamic* mti_Main;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetCurrentTeleportMesh(USkeletalMeshComponent *TeleportMesh);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetRing(int32 RingNumber);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetRingActivationPosition(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsRingCanBeActivated() { return (SelectorValue); };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int32 GetCurrentRing() { return CurrentRing; };
};
