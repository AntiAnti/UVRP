// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TeleportBase.h"
#include "GamePlatform.generated.h"

UCLASS(Blueprintable)
class UVRP_API AGamePlatform : public ATeleportBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGamePlatform();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//TSubobjectPtrDeprecated<
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool ShieldState1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool ShieldState2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool ShieldState3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool ShieldState4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Interaction")
	bool IsPlatformAvalibleForActivation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Interaction")
	bool IsBackdoorTeleport;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Interaction")
	bool IsTrainingLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	TArray<AGamePlatform*> NextPlatforms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	AActor* ParentMechanism;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool IsPassive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	TArray<int32> GuardiansData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool IsTrainingLevelPlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	float PlatformRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	uint8 PlatformNameNumber;

	UPROPERTY(BlueprintReadWrite, Category = "Construct Object")
	bool IsDefenceActivated;

	UFUNCTION(BlueprintCallable, Category = "Service")
	FVector GetProtectedStayLocationC(bool AgainstPlayer, FString SoldierName);

	UFUNCTION(BlueprintCallable, Category = "Platform Interaction")
	FVector GetNearbyLocation(FVector InitLocation, int32 Sector, int32 SectorsCount, float HeightOffset = 500.0f, float HeightDeviation = 150.0f);

	UFUNCTION(BlueprintCallable, Category = "Platform Interaction")
	void GenerateFuturePlatforms(int32 Depth, int32 BranchLevel = 2);

	UFUNCTION(BlueprintCallable, Category = "Platform Interaction")
	void ActivateFuturePlatforms();

	UFUNCTION(BlueprintCallable, Category = "Platform Interaction")
	void ChechPlatformToSpawning(int32 Depth, AGamePlatform* Platform, int32 BranchLevel = 2);

	UFUNCTION(BlueprintCallable, Category = "Platform Interaction")
	void LoadPlatformsGuardians(AGamePlatform *FirstPlatform, FString LastPlatformName);

	UFUNCTION(BlueprintCallable, Category = "Service")
	FVector GetNearestBorderPoint(FVector CurrentLocation);

	UFUNCTION(BlueprintNativeEvent, Category = "Platform Interaction")
	AGamePlatform* SpawnPlatformAtLocation(FVector Location);
	virtual AGamePlatform* SpawnPlatformAtLocation_Implementation(FVector Location);

	UFUNCTION(BlueprintNativeEvent, Category = "Save and Load")
	void SetupTeleportAtLoading(AGamePlatform *TargetPlatform);
	virtual void SetupTeleportAtLoading_Implementation(AGamePlatform *TargetPlatform);

	UFUNCTION(BlueprintNativeEvent, Category = "Save and Load")
	AGamePlatform* GetTeleportTargetPlatform();
	virtual AGamePlatform* GetTeleportTargetPlatform_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	FVector GetSafeLocationC(int32 ShieldNumber);
	virtual FVector GetSafeLocationC_Implementation(int32 ShieldNumber);

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	FVector GetShieldLocation(int32 ShieldNumber);
	virtual FVector GetShieldLocation_Implementation(int32 ShieldNumber) { return FVector::ZeroVector; };

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	void DetachFromBearingMech(bool QuickDetach);
	virtual void DetachFromBearingMech_Implementation(bool QuickDetach) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	void UpdatePlatformShields();
	virtual void UpdatePlatformShields_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	void SetAvalibleForActivation();
	virtual void SetAvalibleForActivation_Implementation() {};

protected:
	FString LocationSoldiers[4];
	bool ShieldsActivated[4];

	UFUNCTION()
	void LoadPlatformsGuardians_R(AGamePlatform *CurrentPlatform, FString LastPlatformName);
};
