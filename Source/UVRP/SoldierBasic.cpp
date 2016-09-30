// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "UVRPGameMode.h"
#include "CharacterVoicePreset.h"
#include "UVRPCharacter.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "SoldierBasic.h"

// Sets default values
ASoldierBasic::ASoldierBasic()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> aeMesh(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/Soldier/sm_Soldier.sm_Soldier'"));
	if (aeMesh.Object != NULL) {
		GetMesh()->SetSkeletalMesh(aeMesh.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));	
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCastShadow(false);

	S_Health = 1.0f;
	S_Hiding = false;
	S_Hiding_ShootingTop = false;
	S_Hiding_ShootingLeft = false;
	S_Hiding_ShootingStanding = false;
	S_EnemyDetectionRadius = 1200.0f;
	S_Koef_Health = 1.35f;
	S_Koef_Temperature = S_Koef_PlayerVisibility = S_Koef_Time = 1.0f;
	S_AccumulatedTime = 0.0f;

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> aeTubeEffect(TEXT("SoundAttenuation'/Game/FirstPerson/Audio/TubeVoiceEffects.TubeVoiceEffects'"));
	if (aeTubeEffect.Object != NULL) {
		SoundEffect = (USoundAttenuation*)aeTubeEffect.Object;
	}
	DamageMultiplier = 1.0f;
}

// Called when the game starts or when spawned
void ASoldierBasic::BeginPlay()
{
	Super::BeginPlay();
	
	gm = (AUVRPGameMode*)UGameplayStatics::GetGameMode(GetWorld());
	if (gm) {
		gm->sr_SoldiersCount++;
		gm->sr_NewSoldierName = this->GetName();

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Soldiers count: " + FString::FromInt(gm->sr_SoldiersCount));
		DamageMultiplier = gm->SoldierDamageMultiplier;
	}

	if (S_VoicePreset != nullptr) {
		S_VoiceStat.Empty();
		for (int i = 0; i < S_VoicePreset->SoundList.Num(); i++) {
			FVoicePlayStatistics item;
			item.FileName = S_VoicePreset->SoundList[i].SoundFile->GetFName().ToString();
			item.QuoteIndex = S_VoicePreset->SoundList[i].QuoteIndex;
			item.PlayedTime = -100.0f;
			S_VoiceStat.Add(item);
		}
	}
	SpeakingInterval = 5.0f;
}

void ASoldierBasic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (gm) {
		gm->sr_SoldiersCount--;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Soldiers count: " + FString::FromInt(gm->sr_SoldiersCount));
	}
}

// Called every frame
void ASoldierBasic::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

// Called to bind functionality to input
void ASoldierBasic::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ASoldierBasic::CheckVoiceAI(float ResultAggression)
{
	if (gm == nullptr) return;

	if (gm->sr_LastShotTime > 30.0f && IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_ISeeYou)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Shoot time: " + FString::SanitizeFloat(gm->sr_LastShotTime));

		S_Speak(EVoiceQuote::VQ_S_ISeeYou, false);
		return;
	}

	// Single
	if (gm->sr_SoldiersCount <= 1) {
		if (gm->sr_NewSoldierName != "") {
			gm->sr_NewSoldierName = "";
			if (IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_HeIsHere)) {
				S_Speak(EVoiceQuote::VQ_S_HeIsHere, false);
				return;
			}
		}
	}
	// Appeals
	if (gm->sr_SoldiersCount > 1) {
		if (gm->sr_NewSoldierName != "") {
			gm->sr_NewSoldierName = "";
			if (IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_KillHim)) {
				S_Speak(EVoiceQuote::VQ_S_KillHim, false);
				return;
			}
		}
		if ((S_Hiding_ShootingTop || S_Hiding_ShootingLeft || S_Hiding_ShootingStanding)) {
			if ((gm->sr_SoldiersCount == 2 || FMath::RandBool()) && IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_CoverMe)) {
				S_Speak(EVoiceQuote::VQ_S_CoverMe, false);
				return;
			}
			else if (IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_HoldHim)) {
				S_Speak(EVoiceQuote::VQ_S_HoldHim, false);
				return;
			}
		}
		if (gm->sr_LastShotTime < 1.0f) {
			if (FMath::RandBool() && IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_HeIsAlive)) {
				S_Speak(EVoiceQuote::VQ_S_HeIsAlive, false);
				return;
			}
			else if (IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_HoldHim)) {
				S_Speak(EVoiceQuote::VQ_S_HoldHim, false);
				return;
			}
		}
		if (gm->sr_LastShotTime > 15.0f && IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_DontLostHim)) {
			S_Speak(EVoiceQuote::VQ_S_DontLostHim, false);
			return;
		}
		if (ResultAggression >= 3.0f && IsVoiceQuoteAvalible(EVoiceQuote::VQ_S_HeSTooFast)) {
			S_Speak(EVoiceQuote::VQ_S_HeSTooFast, false);
			return;
		}
	
	}
}

float ASoldierBasic::GetMinVoiceInterval(EVoiceQuote quote)
{
	float fRet;
	if (quote == EVoiceQuote::VQ_S_ISeeYou)
		fRet = 80.0f;
	else fRet = 40.0f;

	return fRet;
}

inline bool ASoldierBasic::IsVoiceQuoteAvalible(EVoiceQuote quote)
{
	bool bRet;
	float MinInterval = GetMinVoiceInterval(quote);
	float QuoteTime = -100.0f;
	int i;

	for (i = 0; i < S_VoiceStat.Num(); i++)
		if (S_VoiceStat[i].QuoteIndex == quote && QuoteTime < S_VoiceStat[i].PlayedTime)
			QuoteTime = S_VoiceStat[i].PlayedTime;

	float currenttime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	bRet = ((currenttime - QuoteTime) >= MinInterval);

	return bRet;
}

void ASoldierBasic::CheckMainAI(float DeltaTime, float TempInv, float PlVisibility, bool CanShoot)
{
	float ResultAggression = 0;
	const float SPEAK_INTERVAL = 5.0f;
	float a;

	// S_AccumulatedTime += DeltaTime * 0.08f;
	// fix algorithm!
	S_AccumulatedTime = DeltaTime * 0.08f * 2.0f; // *2 added to make soldiers more agressive
	if (S_AccumulatedTime > 1.0f) S_AccumulatedTime = 1.0f;
	SpeakingInterval += DeltaTime;

	// calc aggression
	if (CanShoot) {
		a = S_Health * S_Koef_Health + TempInv * S_Koef_Temperature + PlVisibility * S_Koef_PlayerVisibility + S_AccumulatedTime * S_Koef_Time;
		ResultAggression = 4 * a / (S_Koef_Health + S_Koef_Temperature + S_Koef_PlayerVisibility + S_Koef_Time);
	}
	else {
		ResultAggression = 0.0f;
	}

	// calc behaviour flags
	if (this->GetMovementComponent()->Velocity.Size() == 0) {
		S_Hiding = !(ResultAggression >= 3.75f);

		if (TempInv > 0) {
			S_Hiding_ShootingStanding |= (ResultAggression >= 3 && ResultAggression < 3.75f && TempInv >= 0.3f);
			S_Hiding_ShootingTop |= (ResultAggression >= 2 && ResultAggression < 3 && TempInv >= 0.4f);
			S_Hiding_ShootingLeft |= (ResultAggression >= 1 && ResultAggression < 2 && TempInv >= 0.6f);
		}
	}
	else S_Hiding = false;

	// reset aggression
	if (S_Hiding_ShootingTop || S_Hiding_ShootingLeft || S_Hiding_ShootingStanding) ResultAggression = 0.0f;

	// strategy correction
	if (S_ActionsHistory.Num() >= 3) {
		int i = S_ActionsHistory.Num() - 1;
		if (S_Hiding_ShootingLeft
					&& S_ActionsHistory[i] == ESoldierAIAction::Shooting_From_Left
					&& S_ActionsHistory[i - 1] == ESoldierAIAction::Shooting_From_Left
					&& S_ActionsHistory[i - 2] == ESoldierAIAction::Shooting_From_Left)
		{
			S_Hiding_ShootingLeft = false;
			S_Hiding_ShootingTop = true;
		}
		else if (S_Hiding_ShootingTop
			&& S_ActionsHistory[i] == ESoldierAIAction::Shooting_From_Top
			&& S_ActionsHistory[i - 1] == ESoldierAIAction::Shooting_From_Top
			&& S_ActionsHistory[i - 2] == ESoldierAIAction::Shooting_From_Top)
		{
			S_Hiding_ShootingTop = false;
			S_Hiding_ShootingStanding = true;
		}

	}

	if (SpeakingInterval >= SPEAK_INTERVAL)
		CheckVoiceAI(ResultAggression);
}

bool ASoldierBasic::CanSeePlayer(bool IsSitting)
{
	bool bRet;
	FVector EyesLocation = GetMesh()->GetSocketLocation("EyesSocket");
	if (S_Sitting && !IsSitting) EyesLocation.Z += 96.0f;
	else if (!S_Sitting && IsSitting) EyesLocation.Z -= 96.0f;

	if (!IsValid(PlayerPointer)) {
		PlayerPointer = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	FVector vTargetPoint = PlayerPointer->GetHeadLocation();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.AddIgnoredActor(this);
	RV_TraceParams.AddIgnoredActor(PlayerPointer);
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(RV_Hit, EyesLocation, vTargetPoint, ECollisionChannel::ECC_Visibility, RV_TraceParams);

	bRet = !(RV_Hit.bBlockingHit && RV_Hit.GetActor() != nullptr);

	return bRet;
}

uint8 ASoldierBasic::CanSeePlayerFromPoint(FVector CheckedLocation)
{
	uint8 nRet = 0;
	FVector EyesLocation = GetMesh()->GetSocketLocation("EyesSocket");
	if (S_Sitting) CheckedLocation.Z = EyesLocation.Z + 96.0f; else CheckedLocation.Z = EyesLocation.Z;

	if (!IsValid(PlayerPointer)) {
		PlayerPointer = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}

	FVector vTargetPoint = PlayerPointer->GetHeadLocation();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace_Soldier")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);
	//RV_TraceParams.AddIgnoredActor(PlayerPointer);
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(RV_Hit, CheckedLocation, vTargetPoint, ECollisionChannel::ECC_Visibility, RV_TraceParams);
	if (RV_Hit.bBlockingHit == false && RV_Hit.GetActor() == nullptr) nRet++;

	DrawDebugLine(GetWorld(), CheckedLocation, vTargetPoint, FColor::White, false, 5.0f);

	CheckedLocation.Z -= 96.0f;
	GetWorld()->LineTraceSingleByChannel(RV_Hit, CheckedLocation, vTargetPoint, ECollisionChannel::ECC_Visibility, RV_TraceParams);
	if (RV_Hit.bBlockingHit == false && RV_Hit.GetActor() == nullptr) nRet++;

	DrawDebugLine(GetWorld(), CheckedLocation, vTargetPoint, FColor::White, false, 5.0f);

	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Ret = " + FString::FromInt(nRet));

	return nRet;
}

void ASoldierBasic::S_Speak(EVoiceQuote quote, bool bCheckTime)
{
	float MinInterval = GetMinVoiceInterval(quote);
	FString PlayingFileName;
	int i;
	
	if (S_VoicePreset == nullptr) return;
	USoundBase* sound = S_VoicePreset->GetRandomElement(quote);
	if (sound == nullptr) return;

	PlayingFileName = sound->GetFName().ToString();

	// check last used time
	for (i = 0; i < S_VoiceStat.Num(); i++)
		if (S_VoiceStat[i].FileName == PlayingFileName) break;
	
	if (i < S_VoiceStat.Num()) {
		float currenttime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		if ((currenttime - S_VoiceStat[i].PlayedTime) < MinInterval)
			return;
		else
			S_VoiceStat[i].PlayedTime = currenttime;
	}

	SpeakingInterval = 0.0f;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation(), 1.0f, 1.0f, 0.0f, SoundEffect);
}

void ASoldierBasic::OnHitDamage_Implementation(float Damage, FVector HitPoint, FVector Direction)
{
	FVector t = GetActorLocation();
	FVector rightloc = GetActorRightVector() * 10.0f + t;
	float dist = (HitPoint - t).Size();
	S_Bomb_RightSide = ((HitPoint - rightloc).Size() < dist);

	if (Damage < 0.3f)
		S_Bomb_State = 1;
	else
		S_Bomb_State = 2;
}

void ASoldierBasic::OnSoftDamage_Implementation(float Damage, FVector HitPoint, FVector Direction)
{
	FVector t = GetActorLocation();
	FVector rightloc = GetActorRightVector() * 10.0f + t;
	float dist = (HitPoint - t).Size();
	S_Bomb_RightSide = ((HitPoint - rightloc).Size() < dist);

	S_Bomb_State = 1;
	if (Damage < 0.3f)
		S_Bomb_State = 1;
	else
		S_Bomb_State = 2;
}

