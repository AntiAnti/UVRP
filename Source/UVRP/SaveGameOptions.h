// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "SaveGameOptions.generated.h"

/**
 * 
 */
UCLASS()
class UVRP_API USaveGameOptions : public USaveGame
{
	GENERATED_BODY()
	
public:
	USaveGameOptions();

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bShowHands;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	float fSoundVolume;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	float fVoiceVolume;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	float fMusicVolume;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	uint8 nDifficultyLevel;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	uint8 nGraphicsLevel;	

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAutoDecreaseGraphicsLevel;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementGameFinished;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementExtendedEnding;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementSecretArea1;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementSecretArea2;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementHighDifficulty;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bAchevementDataModuleDecrypted;

	UPROPERTY(VisibleAnywhere, Category = "Service")
	int32 nTrainingMaxScore;

	UPROPERTY(VisibleAnywhere, Category = "Service")
	float fEndingMusicPlayingTime;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bTutorialWasRead;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	bool bGameplayTutorial;

	UPROPERTY(VisibleAnywhere, Category = "Game Options")
	float fFloorOffset;
};
