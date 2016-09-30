// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#define VOLUME_SC_Ambience "SC_Ambience"
#define VOLUME_SC_Music "SC_Music"
#define VOLUME_SC_Sound "SC_Sound"
#define VOLUME_SC_Voice "SC_Voice"

#define DATAMODULE_RED 1
#define DATAMODULE_YELLOW 2
#define DATAMODULE_GREEN 3

#define CHECKFPS_TIME 10
#define CHECKFPS_PERIOD 2

#include "GameFramework/GameMode.h"
#include "UVRPGameMode.generated.h"

/**********************************************************************************************************************/
/**********************************************************************************************************************/

USTRUCT(BlueprintType, Blueprintable)
struct FGraphicsOptions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PortalEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HandsIK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BulletTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PortalRefraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DynamicShadows;
};

USTRUCT(BlueprintType, Blueprintable)
struct FGameplayStatistics
{
	GENERATED_USTRUCT_BODY()

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

UENUM(BlueprintType, Blueprintable)
enum class EGameAchievements : uint8
{
	A_GameEnded			UMETA(DisplayName = "Game Ended"),
	A_ModuleDecrypted	UMETA(DisplayName = "Data Module Decrypted"),
	A_ExtendedEnding	UMETA(DisplayName = "Extended Ending"),
	A_SecretArea1		UMETA(DisplayName = "Secret Area 1"),
	A_SecretArea2		UMETA(DisplayName = "Secret Area 2"),
	A_HighestDifficulty	UMETA(DisplayName = "Highest Difficulty"),
	A_BothSecretAreas	UMETA(DisplayName = "Both Secret Areas")
};


UENUM(BlueprintType, Blueprintable)
enum class EFeedbackAction : uint8
{
	A_PushPhysicalObject	UMETA(DisplayName = "Push Physical Object"),
	A_CatchAndDrop			UMETA(DisplayName = "Catch and Drop"),
	A_UnnaturalLocomotion	UMETA(DisplayName = "Unnatural Locomotion"),
	A_SlingStart			UMETA(DisplayName = "Sling Start"),
	A_SlingPull				UMETA(DisplayName = "Sling Pull"),
	A_Shield				UMETA(DisplayName = "Shield Reflection")
};

USTRUCT(BlueprintType, Blueprintable)
struct FHandFeedbackSetup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLargeServo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultDuration;
};

/**********************************************************************************************************************/
/**********************************************************************************************************************/

UCLASS(minimalapi)
class AUVRPGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUVRPGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// console commands
	UFUNCTION(Exec)
	void invulnerability();

	UFUNCTION(Exec)
	void autosave(int32 mode);

	UFUNCTION(Exec)
	void setplatform(int32 number);

	UFUNCTION(Exec)
	void getplatform();

	UFUNCTION(Exec)
	void save() { SaveGame(true); };

	UFUNCTION(Exec)
	void showstat();

	UFUNCTION(Exec)
	void heal();

	UFUNCTION(Exec)
	void achreset();

	UFUNCTION(Exec)
	void achset(int32 number);

	UFUNCTION(Exec)
	void giveall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelLoadingParams;

	// other
	UFUNCTION()
	bool GetGodMode() { return bGodMode; };

	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void PrepareDataForSaving(UUVRP_MySaveGame* sgi);
	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void LoadSavedDataToLevel(UUVRP_MySaveGame* sgi);

	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void LoadTextFile(FString FileName, FString& Buffer);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SwitchGodMode() { invulnerability(); };

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void OpenJigsaw();

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SaveGameOptions();

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void LoadGameOptions(bool ApplyOptions = true);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SetAudioVolume(FString ClassName, float VolumeMultiplier);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SetDifficultyLevel(uint8 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SetGraphicsLevel(uint8 NewLevel, bool ForceUpdate = false);

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	TArray<FText> ReadTextFromDataTable(int32 DataModuleNumber, FName FileName);
	virtual TArray<FText> ReadTextFromDataTable_Implementation(int32 DataModuleNumber, FName FileName) { TArray<FText> p; return p; };

	UFUNCTION(BlueprintNativeEvent, Category = "Game Options")
	void OnGameLoadingFinished();
	virtual void OnGameLoadingFinished_Implementation() {};


	UFUNCTION(BlueprintNativeEvent, Category = "Steamworks")
	bool Steam_SaveAchievement(EGameAchievements AchievementType);
	virtual bool Steam_SaveAchievement_Implementation(EGameAchievements AchievementType) { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Steamworks")
	bool Steam_SavePlayerTrainingScore();
	virtual bool Steam_SavePlayerTrainingScore_Implementation() { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Steamworks")
	FString Steam_LoadLeaderboard(int32 Offset = 0, int32 ItemsCount = 10);
	virtual FString Steam_LoadLeaderboard_Implementation(int32 Offset = 0, int32 ItemsCount = 10) { return ""; };

	UFUNCTION(BlueprintNativeEvent, Category = "Steamworks")
	void Steam_ResetAchievements();
	virtual void Steam_ResetAchievements_Implementation() {};

public:
	/* Voice reactions global info */
	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FString sr_NewSoldierName;		// <-- filled in bp_Soldier::BeginPlay()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	int32 sr_SoldiersCount;			// <-- filled in ASoldierBasic::BeginPlay() and ASoldierBasic::EndPlay()

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	float sr_LastShotTime;			// <-- filled in UGunComponent::Fire()

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	bool GO_ShowHands;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	float GO_SoundVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	float GO_MusicVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	float GO_VoiceVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	uint8 GO_DifficultyLevel;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	uint8 GO_GraphicsLevel;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	bool GO_AutoDecreaseGraphicsLevel;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	bool GO_GameplayTutorial;

	UPROPERTY()
	uint8 GO_Max_DifficultyLevel;

	UPROPERTY()
	uint8 GO_Max_GraphicsLevel;

	UPROPERTY(BlueprintReadWrite, Category = "Game Options")
	float GO_FloorOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Game Options")
	float SoldierDamageMultiplier;
		
	UPROPERTY(BlueprintReadOnly, Category = "Game Options")
	float TeleportDecryptTimeout;

	UPROPERTY(BlueprintReadOnly, Category = "Game Options")
	FGraphicsOptions GraphicsOptions;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_GameFinished;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_ExtendedEnding;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_SecretArea1;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_SecretArea2;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_HighDifficulty;

	UPROPERTY(BlueprintReadWrite, Category = "Achievements")
	bool Ach_DataModuleDecrypted;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	bool GP_DifficultyLevelChanged;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	bool GP_RunningSoldiersGone;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	int32 GP_TrainingLevelScore;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	float GP_EndingMusicPlayingTime;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	bool GP_TutorialWasRead;

	UPROPERTY(BlueprintReadOnly, Category = "Service")
	bool IsLoadingGame;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Data")
	FGameplayStatistics GameStat;

	///////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION()
	void SetAutoFPSCheck(bool IsEnabled);

	UFUNCTION()
	bool IsAutoFPSCheckActive() { return bAutoFPSCheck; };

	UFUNCTION(BlueprintCallable, Category = "Achievements")
	void UnlockAchievement(EGameAchievements Achievement);

	UFUNCTION(BlueprintCallable, Category = "Achievements")
	bool IsAchievementUnlocked(EGameAchievements Achievement);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Data")
	void StartNewGame(bool Restart = false);

	UFUNCTION(BlueprintCallable, Category = "Service")
	bool IsSaveGameExists();

	UFUNCTION(BlueprintCallable, Category = "Service")
	void LoadGame();

	UFUNCTION(BlueprintCallable, Category = "Service")
	void SaveGame(bool ForceSave = false);

	UFUNCTION(BlueprintCallable, Category = "Service")
	void AutoSaveGame(uint8 ParamType, int32 ParamValue);

	UFUNCTION(BlueprintCallable, Category = "Service")
	void LoadGameStatistics();
	
	UFUNCTION(BlueprintCallable, Category = "Service")
	void SaveGameStatistics();

	UFUNCTION(BlueprintCallable, Category = "Service")
	AMessageWindow* ShowMessage(FText Text, float LifeSpan = 20.0f);

	UFUNCTION(BlueprintCallable, Category = "Service")
	AMessageWindow* ShowDataWindow(int32 DataModuleNumber, FName FileName, AActor* Parent, FTransform RelativeTransform);

	UFUNCTION(BlueprintCallable, Category = "Service")
	FHandFeedbackSetup GetControllerFeedbackOptions(EFeedbackAction Action);

	UFUNCTION(BlueprintNativeEvent, Category = "Service")
	void CalibrateFloor();
	virtual void CalibrateFloor_Implementation() {};

protected:
	UPROPERTY()
	bool bGodMode;

	UPROPERTY()
	bool bAutoSave;

	UPROPERTY()
	bool bAutoFPSCheck;

	UPROPERTY()
	TArray<FGraphicsOptions> GraphicsOptionsLevels;

	UPROPERTY()
	FTimerHandle hTimerCheckFPS;

	UPROPERTY()
	FTimerHandle hTimerLoadLevel;

	UPROPERTY()
	uint8 LastDeltaTime[CHECKFPS_TIME * CHECKFPS_PERIOD];

	UPROPERTY()
	uint8 CurrentFPSIndex;

	UPROPERTY()
	uint32 LastGamePlatform;

	UPROPERTY()
	TMap<EFeedbackAction, FHandFeedbackSetup> FeedbackActions;
	/*TArray<FHandFeedbackSetup> FeedbackActions;*/

	UFUNCTION()
	void Timer_CheckFPS();

	UFUNCTION()
	void Timer_LoadLevel();
};



