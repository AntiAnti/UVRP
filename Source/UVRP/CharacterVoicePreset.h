// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CharacterVoicePreset.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EVoiceQuote : uint8
{
	VQ_S_HeIsHere			UMETA(DisplayName = "S_HeIsHere"),		/* used */
	VQ_S_HeIsAlive			UMETA(DisplayName = "S_HeIsAlive"),		/* used */
	VQ_S_DontLostHim		UMETA(DisplayName = "S_DontLostHim"),	/* used */
	VQ_S_ImHurt				UMETA(DisplayName = "S_ImHurt"),		/* used */
	VQ_S_ImHurtHardly		UMETA(DisplayName = "S_ImHurtHardly"),	/* used */
	VQ_S_CoverMe			UMETA(DisplayName = "S_CoverMe"),		/* used */
	VQ_S_NeedHelp			UMETA(DisplayName = "S_NeedHelp"),		/* used */
	VQ_S_HoldHim			UMETA(DisplayName = "S_HoldHim"),		/* used */
	VQ_S_KillHim			UMETA(DisplayName = "S_KillHim"),		/* used */
	VQ_S_5Tube				UMETA(DisplayName = "S_5Tube"),
	VQ_S_ActivateTurrets	UMETA(DisplayName = "S_ActivateTurrets"),
	VQ_S_ISeeYou			UMETA(DisplayName = "S_ISeeYou"),		/* used */
	VQ_S_HeSTooFast			UMETA(DisplayName = "S_HeSTooFast"),	/* used */
	VQ_S_Dying				UMETA(DisplayName = "S_Dying"),
	VQ_S_BlockElevator		UMETA(DisplayName = "S_BlockElevator"),
	VQ_S_DamageScream		UMETA(DisplayName = "S_DamageScream"),

	/* GLOBAL */

	VQ_C_Death				UMETA(DisplayName = "C_Death"),

	/* INTRO SCENE */

	VQ_C_Intro_Start					UMETA(DisplayName = "C_Intro_Start"),
	VQ_C_Intro_OrderFindCollector		UMETA(DisplayName = "C_Intro_OrderFindCollector"),
	VQ_C_Intro_TeleportFront			UMETA(DisplayName = "C_Intro_TeleportFront"),
	VQ_C_Intro_TeleportLeft				UMETA(DisplayName = "C_Intro_TeleportLeft"),
	VQ_C_Intro_TeleportRight			UMETA(DisplayName = "C_Intro_TeleportRight"),
	VQ_C_Intro_TeleportInactive1		UMETA(DisplayName = "C_Intro_TeleportInactive1"),
	VQ_C_Intro_TeleportInactive2		UMETA(DisplayName = "C_Intro_TeleportInactive2"),
	VQ_C_Intro_ConsoleInactive			UMETA(DisplayName = "C_Intro_ConsoleInactive"),
	VQ_C_Intro_TeleportClean			UMETA(DisplayName = "C_Intro_TeleportClean"),
	VQ_C_Intro_TeleportManually			UMETA(DisplayName = "C_Intro_TeleportManually"),
	VQ_C_Intro_CoilsFront				UMETA(DisplayName = "C_Intro_CoilsFront"),
	VQ_C_Intro_CoilsLeft				UMETA(DisplayName = "C_Intro_CoilsLeft"),
	VQ_C_Intro_CoilsRight				UMETA(DisplayName = "C_Intro_CoilsRight"),
	VQ_C_Intro_CoilsWrong				UMETA(DisplayName = "C_Intro_CoilsWrong"),
	VQ_C_Intro_StepToTeleport			UMETA(DisplayName = "C_Intro_StepToTeleport"),
	VQ_C_Intro_ConnectingAfterTeleport	UMETA(DisplayName = "C_Intro_ConnectingAfterTeleport"),	
	VQ_C_Intro_FindTerminal				UMETA(DisplayName = "C_Intro_FindTerminal"),
	VQ_C_Intro_TerminalFront			UMETA(DisplayName = "C_Intro_TerminalFront"),
	VQ_C_Intro_TerminalLeft				UMETA(DisplayName = "C_Intro_TerminalLeft"),
	VQ_C_Intro_TerminalRight			UMETA(DisplayName = "C_Intro_TerminalRight"),
	VQ_C_Intro_ChooseCollector			UMETA(DisplayName = "C_Intro_ChooseCollector"),
	VQ_C_Intro_CollectorWrong1			UMETA(DisplayName = "C_Intro_CollectorWrong1"),
	VQ_C_Intro_CollectorWrong2			UMETA(DisplayName = "C_Intro_CollectorWrong2"),
	VQ_C_Intro_TeleportToCollector		UMETA(DisplayName = "C_Intro_TeleportToCollector"),

	/* MAIN LEVEL */

	VQ_C_Main_Start1					UMETA(DisplayName = "C_Main_Start1"),
	VQ_C_Main_Start2					UMETA(DisplayName = "C_Main_Start2"),
	VQ_C_Main_SoldiersWarning			UMETA(DisplayName = "C_Main_SoldiersWarning"),
	VQ_C_Main_NewStrategy				UMETA(DisplayName = "C_Main_NewStrategy"),
	VQ_C_Main_MoveFaster				UMETA(DisplayName = "C_Main_MoveFaster"),
	VQ_C_Main_SequireHatch				UMETA(DisplayName = "C_Main_SequireHatch"),
	VQ_C_Main_Jump1						UMETA(DisplayName = "C_Main_Jump1"),
	VQ_C_Main_Jump2						UMETA(DisplayName = "C_Main_Jump2"),
	VQ_C_Main_Emotions					UMETA(DisplayName = "C_Main_Emotions"),
	VQ_C_Main_ToRoller					UMETA(DisplayName = "C_Main_ToRoller"),

	/* ENDING SCENE */

	VQ_C_Ending_Put1					UMETA(DisplayName = "C_Ending_Put1"),
	VQ_C_Ending_Put2					UMETA(DisplayName = "C_Ending_Put2"),
	VQ_C_Ending_Charge1					UMETA(DisplayName = "C_Ending_Charge1"),
	VQ_C_Ending_Charge2					UMETA(DisplayName = "C_Ending_Charge2"),

	/*--------------------------------------------------------------------------------------------*/

	/* GLOBAL 2 */

	VO_C_Gl_Harpoon1					UMETA(DisplayName = "C_Gl_Harpoon1"),
	VO_C_Gl_HarpoonHard					UMETA(DisplayName = "C_Gl_HarpoonHard"),
	VO_C_Gl_Harpoon_Weak				UMETA(DisplayName = "C_Gl_Harpoon_Weak"),
	VO_C_Gl_HarpoonEarly				UMETA(DisplayName = "C_Gl_HarpoonEarly"),
	VO_C_Gl_Grenades					UMETA(DisplayName = "C_Gl_Grenades"),
	VO_C_Gl_PortalCodeUnlocked			UMETA(DisplayName = "C_Gl_PortalCodeUnlocked"),
	VO_C_Gl_NonMissionDataModule1		UMETA(DisplayName = "C_Gl_NonMissionDataModule1"),
	VO_C_Gl_NonMissionDataModule2		UMETA(DisplayName = "C_Gl_NonMissionDataModule2"),
	VO_C_Gl_MissionDataModule			UMETA(DisplayName = "C_Gl_MissionDataModule"),
	VO_C_Gl_ReadModuleInstruction		UMETA(DisplayName = "C_Gl_ReadModuleInstruction"),
	VO_C_Gl_ModuleToTerminal			UMETA(DisplayName = "C_Gl_ModuleToTerminal"),
	VO_C_Gl_ModuleDecrypted				UMETA(DisplayName = "C_Gl_ModuleDecrypted"),
	VO_C_Gl_NotEnoughCodes				UMETA(DisplayName = "C_Gl_NotEnoughCodes"),
	VO_C_Gl_EnoughCodes					UMETA(DisplayName = "C_Gl_EnoughCodes"),
	VO_C_Gl_DecryptProcessWarning1		UMETA(DisplayName = "C_Gl_DecryptProcessWarning1"),
	VO_C_Gl_DecryptProcessWarning2		UMETA(DisplayName = "C_Gl_DecryptProcessWarning2"),
	VO_C_Gl_DecryptProcessFinished		UMETA(DisplayName = "C_Gl_DecryptProcessFinished"),

	/* INTRO 2 */

	VO_C_Intro_ActivateDisabledTeleport	UMETA(DisplayName = "C_Intro_ActivateDisabledTeleport"),
	VO_C_Intro_ClearFirstTeleport		UMETA(DisplayName = "C_Intro_ClearFirstTeleport"),
	VO_C_Intro_HealthBox				UMETA(DisplayName = "C_Intro_HealthBox"),
	VO_C_Intro_OpeningLocks				UMETA(DisplayName = "C_Intro_OpeningLocks"),
	VO_C_Intro_OpenHealthBox			UMETA(DisplayName = "C_Intro_OpenHealthBox"),
	VO_C_Intro_Use_Medpak				UMETA(DisplayName = "C_Intro_Use_Medpak"),
	VO_C_Intro_ActivatingFirstTeleport	UMETA(DisplayName = "C_Intro_ActivatingFirstTeleport"),
	VO_C_Intro_DecryptTeleport1			UMETA(DisplayName = "C_Intro_DecryptTeleport1"),
	VO_C_Intro_DecryptTeleport2			UMETA(DisplayName = "C_Intro_DecryptTeleport2"),
	VO_C_Intro_DecryptTeleport3			UMETA(DisplayName = "C_Intro_DecryptTeleport3"),

	/* ENDING SCENE 2 */

	VO_C_Ending_Hello_DL1				UMETA(DisplayName = "C_Ending_Hello_DL1"),
	VO_C_Ending_Hello_DL2				UMETA(DisplayName = "C_Ending_Hello_DL2"),
	VO_C_Ending_WrondModule				UMETA(DisplayName = "C_Ending_WrondModule"),
	VO_C_Ending_RightModule				UMETA(DisplayName = "C_Ending_RightModule"),
	VO_C_Ending_Plans1					UMETA(DisplayName = "C_Ending_Plans1"),
	VO_C_Ending_Plans2					UMETA(DisplayName = "C_Ending_Plans2"),

	VO_C_Intro_ClearFirstTeleportShort	UMETA(DisplayName = "C_Intro_ClearFirstTeleportShort")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoiceQuotePlayingStart, EVoiceQuote, PlayingPhrase);

USTRUCT(BlueprintType, Blueprintable)
struct FVoiceItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoiceQuote QuoteIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* SoundFile;
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumIntervalTime;
	*/
};

USTRUCT(BlueprintType, Blueprintable)
struct FVoiceSequenceItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoiceQuote Phrase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlayedState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NextRecord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ContinueInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ManualSetup;
};

UCLASS(Blueprintable)
class UVRP_API ACharacterVoicePreset : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterVoicePreset();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:

	UPROPERTY(BlueprintReadWrite, Category = "Global")
	USceneComponent* RootMesh;
	/*
	UPROPERTY(EditDefaultsOnly)
	class UBillboardComponent* BillboardMesh;
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Global")
	UAudioComponent* PlayAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TArray<FVoiceItem> SoundList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TArray<FVoiceSequenceItem> SequenceList;

	UPROPERTY(BlueprintAssignable)
	FVoiceQuotePlayingStart OnQuotePlayingStart;

	UFUNCTION(BlueprintCallable, Category = "Global")
	USoundBase* GetRandomElement(EVoiceQuote QuoteIndex);

	UFUNCTION(BlueprintCallable, Category = "Global")
	float PlayElementByIndex(int32 Index, bool ForcePlay = true);

	UFUNCTION(BlueprintCallable, Category = "Global")
	float PlayElementByQuote(EVoiceQuote Quote, bool ForcePlay = true) { return PlayElementByIndex(GetSequenceIndexByQuote(Quote), ForcePlay); };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetSequenceBranchStateByIndex(int32 Index, bool IsPlayed);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetSequenceBranchStateByQuote(EVoiceQuote Quote, bool IsPlayed) { SetSequenceBranchStateByIndex(GetSequenceIndexByQuote(Quote), IsPlayed); };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void StopPlaying() { if (IsValid(PlayAudioComponent)) PlayAudioComponent->Stop(); NowPlayingIndex = -1; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool IsQuotePlayed(EVoiceQuote Quote) { return SequenceList[GetSequenceIndexByQuote(Quote)].PlayedState; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void ResetPreset();

protected:
	UPROPERTY()
	int32 NowPlayingIndex;

	UPROPERTY()
	int32 NextIndex;

	UPROPERTY()
	FTimerHandle hBranchPlayingTimer;

	UPROPERTY()
	FTimerHandle hCurrentPlayingTimer;

	UPROPERTY()
	bool SwitchToNextQuote;

	UFUNCTION()
	int32 GetSequenceIndexByQuote(EVoiceQuote Quote);

	UFUNCTION()
	void OnTimerPlayBranch();

	UFUNCTION()
	void OnTimerPlayCurrent();
};
