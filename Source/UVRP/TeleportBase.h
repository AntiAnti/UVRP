// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TeleportBase.generated.h"

class AUVRPGameMode;

UCLASS(Blueprintable)
class UVRP_API ATeleportBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleportBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construct Object")
	bool IsPlayerCanReturnToThePlatform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room-scale")
	FVector RoomCentreLocation;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Gameplay")
	ATeleport* GetTeleport();
	virtual ATeleport* GetTeleport_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Room-scale")
	FVector GetPlayerCentreLocation();

private:
	UPROPERTY()
	AUVRPGameMode* ActiveGameMode;
};
