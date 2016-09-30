// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "SoldierBasic.generated.h"

class AUVRPCharacter;

UENUM(BlueprintType, Blueprintable)
enum class ESoldierAIAction : uint8
{
	Shooting_Staying			UMETA(DisplayName = "Shooting Staying"),
	Shooting_From_Top			UMETA(DisplayName = "Shooting from Top"),
	Shooting_From_Left			UMETA(DisplayName = "Shooting from Left"),
	Shooting_From_Right			UMETA(DisplayName = "Shooting from Right"),
	Shooting_Stanging			UMETA(DisplayName = "Shooting Standing")
};

UENUM(BlueprintType, Blueprintable)
enum class ESoldierPatrolMode : uint8
{
	Walking				UMETA(DisplayName = "Walking"),
	Staying				UMETA(DisplayName = "Staying"),
	StayingNearShield	UMETA(DisplayName = "Staying Near Shield"),
	Sitting				UMETA(DisplayName = "Sitting")
};

USTRUCT(BlueprintType, Blueprintable)
struct FVoicePlayStatistics
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoiceQuote QuoteIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayedTime;
};

UCLASS(Blueprintable)
class UVRP_API ASoldierBasic : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASoldierBasic();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float S_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Hiding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Sitting; //96 cm

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Hiding_ShootingTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Hiding_ShootingLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Hiding_ShootingStanding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 S_Bomb_State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_Bomb_RightSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_HeadFollowBomb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool S_CanFall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<ESoldierAIAction> S_ActionsHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float S_EnemyDetectionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ESoldierPatrolMode S_PatrolMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	TArray<FVoicePlayStatistics> S_VoiceStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voice")
	class ACharacterVoicePreset* S_VoicePreset;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	float DamageMultiplier;

	UPROPERTY()
	USoundAttenuation *SoundEffect;

	UPROPERTY()
	float S_Koef_Health;
	UPROPERTY()
	float S_Koef_Temperature;
	UPROPERTY()
	float S_Koef_PlayerVisibility;
	UPROPERTY()
	float S_Koef_Time;

	UPROPERTY()
	float S_AccumulatedTime;
	
	UPROPERTY()
	class AUVRPGameMode* gm;

	UFUNCTION()
	void CheckVoiceAI(float ResultAggression);

	UFUNCTION()
	float GetMinVoiceInterval(EVoiceQuote quote);

	UFUNCTION()
	inline bool IsVoiceQuoteAvalible(EVoiceQuote quote);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void CheckMainAI(float DeltaTime, float TempInv, float PlVisibility, bool CanShoot);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ResetBombState() {	S_Bomb_State = 0; S_HeadFollowBomb = false; };

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanSeePlayer(bool IsSitting);

	UFUNCTION(BlueprintCallable, Category = "AI")
	uint8 CanSeePlayerFromPoint(FVector CheckedLocation);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void S_Speak(EVoiceQuote quote, bool bCheckTime = true);

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	void OnHitDamage(float Damage, FVector HitPoint, FVector Direction);
	virtual void OnHitDamage_Implementation(float Damage, FVector HitPoint, FVector Direction);

	UFUNCTION(BlueprintNativeEvent, Category = "Damage")
	void OnSoftDamage(float Damage, FVector HitPoint, FVector Direction);
	virtual void OnSoftDamage_Implementation(float Damage, FVector HitPoint, FVector Direction);

private:
	UPROPERTY()
	AUVRPCharacter* PlayerPointer;

	UPROPERTY()
	float SpeakingInterval;
};
