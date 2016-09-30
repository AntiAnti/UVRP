// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "DataJigsaw.h"
#include "HandController.h"
#include "UVRPGameMode.h"
#include "UVRPCharacter.h"
#include "UVRP_MySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlatformStatus {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 Number;

	UPROPERTY(VisibleAnywhere)
	FTransform ObjectTransform;

	UPROPERTY(VisibleAnywhere)
	bool IsAttached;

	UPROPERTY(VisibleAnywhere)
	bool IsProtected;

	UPROPERTY(VisibleAnywhere)
	bool Shield1;
	UPROPERTY(VisibleAnywhere)
	bool Shield2;
	UPROPERTY(VisibleAnywhere)
	bool Shield3;
	UPROPERTY(VisibleAnywhere)
	bool Shield4;

	UPROPERTY(VisibleAnywhere)
	bool IsAvalibleForActivation;
};

USTRUCT(BlueprintType)
struct FSimpleObjectStatus {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 Number;

	UPROPERTY(VisibleAnywhere)
	bool IsExists;

	UPROPERTY(VisibleAnywhere)
	bool OpenStarted;

	UPROPERTY(VisibleAnywhere)
	bool OpenFinished;

	UPROPERTY(VisibleAnywhere)
	bool IsActive;

	UPROPERTY(VisibleAnywhere)
	FTransform ObjectTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
};

USTRUCT(BlueprintType)
struct FDataDeviceStatus {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 Number;

	UPROPERTY(VisibleAnywhere)
	bool IsDecrypted;

	UPROPERTY(VisibleAnywhere)
	TArray<FJigsawElement> JigsawPieces;
};

/**
 * 
 */
UCLASS()
class UVRP_API UUVRP_MySaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UUVRP_MySaveGame(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	float PlayerHealth;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	float PlayerScore;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	float DifficultyLevel;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	bool IsDifficultyLevelChanged;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	bool IsGunLargeActive;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	bool IsGunSlingActive;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	bool IsGunShieldActive;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	bool IsAdvancedShieldActive;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	int32 CurrentPlatformNumber;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	TArray<FPlatformStatus> Platforms;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	TArray<FSimpleObjectStatus> LevelObjects;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	TArray<FDataDeviceStatus> PlayerDataModules;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	TArray<bool> PuzzlePiecesStatus;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	bool RunningSoldiersGone;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	FString CurrentLevelName;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	TArray<bool> VoicePlayingState;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	EControllerTool LeftHandGun;

	UPROPERTY(VisibleAnywhere, Category = "Player Status")
	EControllerTool RightHandGun;

	UPROPERTY(VisibleAnywhere, Category = "Level Status")
	TArray<EHelpMessage> HelpMessages;
};