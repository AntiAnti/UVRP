// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "HandsComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API UHandsComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UHandsComponent();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Setup")
	float DefaultHandScale;
	UPROPERTY(BlueprintReadWrite, Category = "Result")
	float HandScale;

	UPROPERTY(BlueprintReadWrite, Category = "Setup")
	float CollarboneMaxOffset;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FVector LeftHandLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FRotator LeftHandRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FVector RightHandLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FRotator LeftToolRotation;
	UPROPERTY(BlueprintReadWrite, Category = "Input")
	FRotator RightToolRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator LeftCollarboneRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FVector LeftUpperarmLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator LeftUpperarmRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FVector LeftForearmLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator LeftForearmRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator RightCollarboneRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FVector RightUpperarmLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator RightUpperarmRotation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FVector RightForearmLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	FRotator RightForearmRotation;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateHandsMesh();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateHandsMeshType(bool ShowHands);

	UFUNCTION(BlueprintCallable, Category = "Service")
	void CalibrateHandLength(bool RightHand);

private:
	UPROPERTY()
	FVector LeftCollarboneLocation;
	UPROPERTY()
	FVector RightCollarboneLocation;
	UPROPERTY()
	float LenUpperarm;
	UPROPERTY()
	float LenForearm;
	UPROPERTY()
	bool bShowHands;

	UPROPERTY()
	USkeletalMesh* MeshHandsLong;
	UPROPERTY()
	USkeletalMesh* MeshHandsShort;
	//FTransformRCollar

	// главная функция
	UFUNCTION()
	void UpdateHandRotators(FRotator &retUpperarmRotation, FRotator &retForearmRotation, FName _CollarboneName, 
				FName _UpperarmName, FName _ForearmName, FName _HandName, FVector HandLocation, FRotator ToolRotation,
				float lenUpper, float lenFore, float direct, bool hand);

	inline FVector GetFrontVector();

	// математика
	UFUNCTION()
	FVector4 GetPlaneByThreePoints(FVector p1, FVector p2, FVector p3);

	UPROPERTY()
	FVector RotationData_HMD[90];
	UPROPERTY()
	FVector RotationData_Left[90];
	UPROPERTY()
	FVector RotationData_Right[90];
	UPROPERTY()
	FVector MovingData_Left[90];
	UPROPERTY()
	FVector MovingData_Right[90];
	UPROPERTY()
	float   RotationData_Time[90];
	UPROPERTY()
	int32   HistoryId;

	UFUNCTION()
	void SaveCurrentFrameRotation();

public:
	UFUNCTION()
	bool IsObjectRotating();
};
