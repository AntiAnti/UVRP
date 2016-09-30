// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "PlayerShield.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API UPlayerShield : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPlayerShield();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UAnimationAsset *AnimationClose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	UAnimationAsset *AnimationOpen;

	UFUNCTION(BlueprintCallable, Category = "Main")
	bool OpenShield();

	UFUNCTION(BlueprintCallable, Category = "Main")
	bool CloseShield();

	UFUNCTION(BlueprintCallable, Category = "Main")
	bool GetIsOpen() { return bIsOpen; };

	UFUNCTION(BlueprintCallable, Category = "Main")
	bool GetIsShieldActive() { return bIsShieldActive; };

	UFUNCTION(BlueprintNativeEvent, Category = "Main")
	void StateChanged(bool NewState);
	virtual void StateChanged_Implementation(bool NewState);

	UFUNCTION()
	void TimerFunction();

protected:
	UPROPERTY()
	UMaterialInstanceDynamic *mti_ShieldField;

private:
	UPROPERTY()
	bool bIsOpen;
	UPROPERTY()
	bool bIsShieldActive;
	UPROPERTY()
	FTimerHandle hWaitForAnimation;
};