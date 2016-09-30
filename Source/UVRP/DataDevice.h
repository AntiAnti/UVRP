// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "DataModuleViewList.h"
#include "DataJigsaw.h"
#include "DataDevice.generated.h"

class AUVRPCharacter;

/**
 * 
 */
UCLASS()
class UVRP_API ADataDevice : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ADataDevice();

	/* Components */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent *ControllerCollider;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UParticleSystemComponent *ProjectorEmitter;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent *ProjectorPlane;

	/*****************************************************************************************/

	virtual void BeginPlay() override;

	/* Globals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	AActor *ParentLockActor; /* AHandController or any scene AStaticMeshActor */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool IsAvalibleForTake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool IsAvalibleForUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool IsEncrypted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	FColor ItemColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TArray<FDataModuleItem> ModuleDataTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool CanBeThrownOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	int32 UniqueNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	int32 RequiredDataModule; // if data module is not encrypted, but it requires access code from the other module

	/* Gameplay */

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	TArray<FJigsawElement> JigsawData;

	UFUNCTION()
	void SetAvalibleForOpen(bool NewState) {};
	UFUNCTION()
	void ShowProjectedData(bool ShowOrHide) {};

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool AttachToController(AActor *NewParent);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DetachFormController();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetLight(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ThrowDevice(FVector AngularVelocity, FVector LinearVelocity);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetVisibility(bool IsVisible);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsVisible();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetProjectorEnabled(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsProjectorEnabled() { return IsShowingData; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsDeviceAttached() { return IsAttachedToActor; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DisableCollision(USceneComponent* NewParentComponent = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	bool GetJigsawPartAvalibility(uint8 Index);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	uint8 GetJigsawState(uint8 Index);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void SetJigsawPartAvalibility(uint8 Index, bool NewAvalibility);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void SetJigsawState(uint8 Index, uint8 NewState);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void GetJigsawElement(FJigsawElement &RetItem, uint8 Index) { if (Index < JigsawData.Num()) RetItem = JigsawData[Index]; };

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void SetJigsawElement(FJigsawElement Item, uint8 Index) { if (Index < JigsawData.Num()) JigsawData[Index] = Item; };

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	int32 UnlockRandomJigsawElement();

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void OnJigsawPieceComplete(int32 PiecesCompleted);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void OnJigsawComplete();

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	bool CanBeDecrypted();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFalling();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void DeactivateDisplay();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActivateBeamInteraction();

private:
	UPROPERTY()
	bool IsShowingData;
	UPROPERTY()
	bool IsAttachedToActor;
	UPROPERTY()
	UMaterialInstanceDynamic* mti_LightCross;
	UPROPERTY()
	UMaterialInstanceDynamic* mti_DisplayMain;
	UPROPERTY()
	FTimerHandle hThrowTimer;
	UPROPERTY()
	FTimerHandle hJigsawCompleteTimer;
	UPROPERTY()
	FVector PrevLocation;
	UPROPERTY()
	ADataModuleViewList* ActiveListView;
	UPROPERTY()
	ADataJigsaw* CurrentJigsaw;

	UPROPERTY()
	float DisplayOffsetHand;
	UPROPERTY()
	float DisplayOffsetFloor;

	UPROPERTY()
	AUVRPCharacter* PlayerOwner;

	UPROPERTY()
	bool IsDisplayActive;

	UFUNCTION()
	void MainTrigger_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ThrowTimer_Tick();
	UFUNCTION()
	void JigsawCompleteTimer_Tick();

	/*
	UFUNCTION()
	void Trigger1_OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(0, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger1_OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(0, false, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger2_OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(1, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger2_OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(1, false, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger3_OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(2, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger3_OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(2, false, OtherActor, OtherComp); };
	*/
};
