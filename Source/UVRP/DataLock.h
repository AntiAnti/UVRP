// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef TAG_DataLock
#define TAG_DataLock "DataLock"
#endif

#include "Engine/StaticMeshActor.h"
#include "DataDevice.h"
#include "DataLock.generated.h"

/**
 * 
 */
UCLASS()
class UVRP_API ADataLock : public AStaticMeshActor, public IMenuInteractionInterface
{
	GENERATED_BODY()
	
public:
	ADataLock();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/*
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	AActor *LockOwnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool IsLockEnabledOnStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool IsWiresMeshVisible;

	UPROPERTY(VisibleDefaultsOnly, Category = "Global")
	UStaticMeshComponent *WiresMesh;
	/*
	UPROPERTY(VisibleDefaultsOnly, Category = "Global")
	UBoxComponent *ExternalTrigger;
	*/
	UPROPERTY(VisibleDefaultsOnly, Category = "Global")
	UBoxComponent *PositionalTrigger1;
	UPROPERTY(VisibleDefaultsOnly, Category = "Global")
	UBoxComponent *PositionalTrigger2;
	UPROPERTY(VisibleDefaultsOnly, Category = "Global")
	UBoxComponent *PositionalTrigger3;

	UFUNCTION(BLueprintCallable, Category = "Global")
	void SetLight(bool Value);

	UFUNCTION()
	void SetControllerConnection(bool Value, AHandController *controller);

	UFUNCTION()
	void OnDataDeviceAttached(bool IsAttachedOrDetached = true);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool OnClick(FVector HitPoint) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	ADataDevice *ChildDataModule;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	AHandController* GetActiveController() { return ActiveController; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DisableLock() { IsLockEnabledOnStart = false; SetLight(false); };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool HasDataModule() { return IsValid(ChildDataModule); };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetWiresVisibility(bool IsVisible);

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool CanAcceptDataDevice() { return IsCanAcceptDataDevice; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetWiresLight(bool Value);

private:
	UPROPERTY()
	AHandController *ActiveController;
	UPROPERTY()
	UMaterialInstanceDynamic *mti_LightLockL0;
	UPROPERTY()
	UMaterialInstanceDynamic *mti_LightLockL1;
	UPROPERTY()
	UMaterialInstanceDynamic *mti_LightLockL2;
	UPROPERTY()
	UMaterialInstanceDynamic *mti_WiresLockL0;
	UPROPERTY()
	UMaterialInstanceDynamic *mti_WiresLockL1;

	UPROPERTY()
	bool ControllerConnected;
	UPROPERTY()
	bool IsCanAcceptDataDevice;

	UPROPERTY()
	float TurnsPerSecond;
	UPROPERTY()
	float CurrentCircle;
	UPROPERTY()
	FTimerHandle hTimerReactivate;

	UFUNCTION()
	void Timer_Reactivate();

	/*
	UFUNCTION()
	void External_OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void External_OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	*/
};
