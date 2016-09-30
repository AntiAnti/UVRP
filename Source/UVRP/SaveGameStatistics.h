// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "UVRPGameMode.h"
#include "SaveGameStatistics.generated.h"

/**
 * 
 */
UCLASS()
class UVRP_API USaveGameStatistics : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameStatistics();

	/*
	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	FGameplayStatistics GameStat;
	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Killed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Injured;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Died;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeleportsUnlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CodesReceived;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InGameTime;
};
