// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#define __IS_BUILD_FINAL true

#include "GameFramework/Character.h"
#include "CharacterVoicePreset.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Engine/SceneCapture2D.h"
#include "UVRPCharacter.generated.h"

/**********************************************************************************************************************/
/**********************************************************************************************************************/

class UInputComponent;
class AHandController;
class ADataDevice;
class ATeleportBase;
class ACharacterVoicePreset;
class AUVRPGameMode;
class AMessageWindow;

USTRUCT(BlueprintType, Blueprintable)
struct FBackgroundAudioParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VolumeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicFile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeoutInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 LoopsCount;
};

UENUM(BlueprintType, Blueprintable)
enum class EHelpMessage : uint8
{
	M_DataModuleTaken				UMETA(DisplayName = "Data Module Taken"),
	M_DataModuleDroppedEnabled		UMETA(DisplayName = "Data Module Dropped Enabled"),
	M_DataModuleDroppedDisabled		UMETA(DisplayName = "Data Module Dropped Disabled")
};

/**********************************************************************************************************************/
/**********************************************************************************************************************/

UCLASS(config=Game)
class AUVRPCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	class UHandsComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay")
	class UArrowComponent* GunArrow;

	UPROPERTY(VisibleDefaultsOnly, Category = "Display")
	class UStaticMeshComponent* LeftHandDisplay;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USteamVRChaperoneComponent* SteamVRChaperone;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	class USceneComponent* CameraBase;
	
public:
	AUVRPCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	class USceneComponent* ControllersRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	class UMotionControllerComponent* MControllerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	class UMotionControllerComponent* MControllerRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	class UChildActorComponent *MHandToolLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	class UChildActorComponent *MHandToolRight;
	*/

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AUVRPProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice")
	class UAudioComponent* PlayVoiceComponent;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FName CurrentPlatformName;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FString CurrentLevelName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 GameScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Control")
	class ATeleportBase* CurrentLocationBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	class ACharacterVoicePreset* AI_VoicePreset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data Device")
	TArray<ADataDevice*> ControllerDataDevices;

	UPROPERTY(BlueprintReadWrite, Category = "Teleport")
	AActor* PortalCaptureTarget;

	UPROPERTY(BlueprintReadWrite, Category = "Teleport")
	ASceneCapture2D* PortalCaptureCamera;

	UPROPERTY(BlueprintReadWrite, Category = "Teleport")
	ATeleportBase* TargetPortalPlatform;

	UPROPERTY(BlueprintReadWrite, Category = "Teleport")
	bool IsPortalVisible;

	UPROPERTY(BlueprintReadWrite, Category = "Teleport")
	bool CanShowTeleportPortal;

	UPROPERTY(BlueprintReadOnly, Category = "Display")
	UMaterialInstanceDynamic* mti_HandDisplay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DamageFadeInTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DamageFadeOutTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float DamageAlphaMultiplier;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	float DamageMultiplier;

	UPROPERTY(BlueprintReadWrite, Category = "Audio")
	FBackgroundAudioParams BGMusicParams;

	UPROPERTY(BlueprintReadWrite, Category = "Voice")
	ACharacterVoicePreset* VoicePreset;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	bool CanOpenMainMenu;

	UPROPERTY()
	TArray<EHelpMessage> HelpMessagesUsed;

	UPROPERTY(BlueprintReadWrite, Category = "Gameplay")
	bool IsInTrainingLevel;

private:
	UPROPERTY()
	TArray<bool> ControllerDataDevicesAvailability;

	UPROPERTY()
	FTimerHandle BGMusicTimer;

	UPROPERTY()
	UAudioComponent* BGAudioComp;

	UPROPERTY()
	float BGAudioPlayingPosition;
	UPROPERTY()
	float BGAudioPlayingStartTime;

	UPROPERTY()
	USoundAttenuation* SoundEffects;

	UPROPERTY()
	AUVRPGameMode* ActiveGameMode;

	UPROPERTY()
	float GameplayStatTime;

	UPROPERTY()
	APlayerController* MyController;
	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data Device")
	int32 CurrentDataDeviceIndex;
	*/
	/***************************************** FUNCTIONS *************************************************/
public:
	void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "Data Device")
	void TakeDataDevice(ADataDevice *NewDevice);

	UFUNCTION(BlueprintCallable, Category = "Data Device")
	bool PutDataDevice(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Data Device")
	void TakeDataDeviceAuto(ADataDevice *NewDevice, bool RightHand = true, bool KeepDeviceVisible = false);

	UFUNCTION(BlueprintCallable, Category = "Data Device")
	void SetDataDeviceActive(int32 CurrentIndex, int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category = "Data Device")
	int32 GetNextDataDeviceIndex(int32 CurrentIndex);

	UFUNCTION(BlueprintCallable, Category = "Body Tracking")
	FVector GetHeadLocation();

	UFUNCTION(BlueprintCallable, Category = "Body Tracking")
	FVector GetFootLocation();

	UFUNCTION(BlueprintCallable, Category = "Body Tracking")
	FVector GetLeftHandLocation();

	UFUNCTION(BlueprintCallable, Category = "Body Tracking")
	FVector GetRightHandLocation();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsAlive() { return (Health > 0); };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Damage(float Value);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Heal(float Value);

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void PlayerDied();
	virtual void PlayerDied_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetTimeWrap();

	UFUNCTION(BlueprintCallable, Category = "Game Control")
	bool TeleportToLevel(FString LevelName);

	UFUNCTION(BlueprintCallable, Category = "Game Control")
	void OpenMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game Control")
	void ReturnFromMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	float PlayVoice(EVoiceQuote quote, bool force = true);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void StopPlayingVoice() { if (IsValid(VoicePreset)) VoicePreset->StopPlaying(); };

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	void AttachActorToHand(AActor *MenuActor, FName SocketName);
	void AttachActorToHand_Implementation(AActor *MenuActor, FName SocketName);

	UFUNCTION(BlueprintCallable, Category = "Controller Tools")
	void SetGunSmallActive(bool IsActive = true);

	UFUNCTION(BlueprintCallable, Category = "Controller Tools")
	void SetGunLargeActive(bool IsActive = true);
	
	UFUNCTION(BlueprintCallable, Category = "Controller Tools")
	void SetGunSlingActive(bool IsActive = true);

	UFUNCTION(BlueprintCallable, Category = "Controller Tools")
	void SetShieldActive(bool IsActive = true);

	UFUNCTION(BlueprintCallable, Category = "Controller Tools")
	void SetAdvancedShieldActive(bool IsActive = true);

	UFUNCTION(BlueprintCallable, Category = "Basic")
	virtual FRotator GetViewRotation() const override;

	UFUNCTION()
	AHandController* GetRightController();

	UFUNCTION()
	AHandController* GetLeftController();

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void SetHandMeshVisibility(bool RightHand, bool IsVisible);

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	FVector GetControllerAngularVelocity(bool RightHand);
	virtual FVector GetControllerAngularVelocity_Implementation(bool RightHand) { return FVector::ZeroVector; };

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	FVector GetControllerLinearVelocity(bool RightHand);
	virtual FVector GetControllerLinearVelocity_Implementation(bool RightHand) { return FVector::ZeroVector; };

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	FVector GetControllerVelocity(bool RightHand);
	virtual FVector GetControllerVelocity_Implementation(bool RightHand) { return FVector::ZeroVector; };

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	FTransform GetControllerHandTransform(bool RightHand);
	virtual FTransform GetControllerHandTransform_Implementation(bool RightHand) { return FTransform(); };

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void CameFormInterLevel();
	virtual void CameFormInterLevel_Implementation() {};

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void SetControllersType(bool IsSkeletalMesh);

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void OnSetControllersType(bool IsSkeletalMesh);
	virtual void OnSetControllersType_Implementation(bool IsSkeletalMesh) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void DamageReceived(float Value);
	virtual void DamageReceived_Implementation(float Value) {};

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void OnStartTeleport(ATeleportBase* TargetPlatform);

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void OnChangeTeleportPlatform(ATeleportBase* NewTargetPlatform);

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void OnEndTeleportPlatform();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsPlayerReturningFromMenu() { return IsReturningFromMenu; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsInMainMenu() { return (IsGamePaused && UGameplayStatics::GetCurrentLevelName(GetWorld()) != TEXT("demolevel")); };

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void UpdateHandsView(bool HandsView);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StartBackgroundMusic(FBackgroundAudioParams MusicParams);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopBackgroundMusic(FBackgroundAudioParams MusicParams);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopBackgroundMusicHardly();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void AllowThrowMainDataDevice();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	USoundAttenuation* GetSoundAttenuation() { return SoundEffects; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool GetIsGameActive() { return IsGameActive; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetIsGameActive(bool IsInGame) { IsGameActive = IsInGame; };

	UFUNCTION(BlueprintNativeEvent, Category = "Game Control")
	void OnPlatformBeginManually(AGamePlatform* NewGamePlatform);
	virtual void OnPlatformBeginManually_Implementation(AGamePlatform* NewGamePlatform) {};

	UFUNCTION()
	void ResetGameplayTime() { GameplayStatTime = 0.0f; };

	UFUNCTION()
	void SetupGameplayTime(bool StartOrPause);

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void PlayFeedback(float Intensity = 0.5f, float Duration = 0.3f, bool LeftLarge = false, bool LeftSmall = false, bool RightLarge = false, bool RightSmall = false, bool IsStartOrStop = true);

	UFUNCTION(BlueprintNativeEvent, Category = "Controllers")
	void PlayFeedbackByAction(float Intensity, float Duration, bool LeftLarge, bool LeftSmall, bool RightLarge, bool RightSmall, EDynamicForceFeedbackAction::Type Action);
	virtual void PlayFeedbackByAction_Implementation(float Intensity, float Duration, bool LeftLarge, bool LeftSmall, bool RightLarge, bool RightSmall, EDynamicForceFeedbackAction::Type Action);

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void PlaySingleFeedback(EFeedbackAction Action, bool IsRightHand);

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	void UpdateFeedbackByAction(FHandFeedbackSetup Options, bool IsRightHand, EDynamicForceFeedbackAction::Type FeedBackAction);

	UFUNCTION(BlueprintCallable, Category = "Controllers")
	bool HasDataDevice(int32 UniqueNumber);

	UFUNCTION()
	void OnDataDeviceSelected(int32 UniqueNumber);

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ShowHelpMessage(EHelpMessage MessageType);

protected:
	UPROPERTY()
	bool IsGamePaused;

	UPROPERTY()
	bool IsReturningFromMenu;

	UPROPERTY()
	bool IsGameActive;

	UPROPERTY()
	FTimerHandle TimerReturnFromMenu;

	UPROPERTY()
	UReverbEffect *TubeEchoEffect;

	UPROPERTY()
	FVector PlayerBaseOffset;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	float InitialHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AHandController *LeftHandToolController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	AHandController *RightHandToolController;

	UPROPERTY()
	float TimeWrapMultiplier;

	UPROPERTY()
	FTimerHandle hTimeWrapTimer;

	UPROPERTY()
	USoundBase* TimeWrapEffect;

	UPROPERTY()
	AMessageWindow* LastMessageWindow;

	UFUNCTION()
	void ResetControllerOrientation();

	UFUNCTION()
	void ResetMenuReturningFlag();

	UFUNCTION(BlueprintCallable, Category = "MenuInteractionInterface")
	void TranslateMenuInterfaceEvent(AActor *Target, FVector HitPoint);

	UFUNCTION()
	void Timer_BackgroundMusic();

	UFUNCTION()
	FVector CalculateMainLevelOffest();

	UFUNCTION()
	void KillPlayer();

	UPROPERTY()
	float HeadOffsetTime;

	UPROPERTY()
	float HeadOffsetValue;

	UPROPERTY()
	FTimerHandle hTimerKillPlayer;

	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);
public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class UHandsComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

