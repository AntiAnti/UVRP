// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "CharacterVoicePreset.h"


// Sets default values
ACharacterVoicePreset::ACharacterVoicePreset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* RootMesh = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = RootMesh;
	//BillboardMesh = CreateDefaultSubobject<UBillboardComponent>("BillboardRoot");
	//BillboardMesh-> ->AttachParent = RootComponent;

	//BillboardMesh = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("BillboardMesh");
	//BillboardMesh->AttachParent = RootComponent;
	//BillboardMesh->SetRelativeLocation(FVector::ZeroVector);
	//MenuItemComponent[i]->SetMaterial(1, MainMaterialInstance);
	//BillboardMesh->SetVisibility(true);
	SwitchToNextQuote = false;
	NowPlayingIndex = -1;
}

// Called when the game starts or when spawned
void ACharacterVoicePreset::BeginPlay()
{
	Super::BeginPlay();
	NowPlayingIndex = -1;
}

// Called every frame
void ACharacterVoicePreset::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

USoundBase* ACharacterVoicePreset::GetRandomElement(EVoiceQuote QuoteIndex)
{
	TArray<USoundBase*> out;
	
	for (int i = 0; i < SoundList.Num(); i++) {
		if (SoundList[i].QuoteIndex == QuoteIndex)
			out.Add(SoundList[i].SoundFile);
	}

	if (out.Num() > 0) {
		int32 i = FMath::RandRange(0, out.Num() - 1);
		return out[i];
	}
	else
		return nullptr;
}

float ACharacterVoicePreset::PlayElementByIndex(int32 Index, bool ForcePlay)
{
	float fRet = 0.0f;
	if (Index < 0 || Index >= SequenceList.Num()) return fRet;
	if (NowPlayingIndex == Index) return fRet;

	//UE_LOG(LogTemp, Warning, TEXT("ACharacterVoicePreset::PlayElementByIndex"));

	if (IsValid(PlayAudioComponent)) {
		// reset timer
		if (hBranchPlayingTimer.IsValid()) {
			GetWorldTimerManager().ClearTimer(hBranchPlayingTimer);
			hBranchPlayingTimer.Invalidate();
		}
		if (hCurrentPlayingTimer.IsValid()) {
			if (ForcePlay) {
				GetWorldTimerManager().ClearTimer(hCurrentPlayingTimer);
				hCurrentPlayingTimer.Invalidate();
				if (NowPlayingIndex >= 0) {
					if (!SequenceList[NowPlayingIndex].ManualSetup) SequenceList[NowPlayingIndex].PlayedState = true;
				}
			}
			else {
				NextIndex = Index;
				SwitchToNextQuote = true;
				return fRet;
			}
		}

		// check if already played
		while (Index < SequenceList.Num() && SequenceList[Index].PlayedState && SequenceList[Index].NextRecord > 0) {
			Index += SequenceList[Index].NextRecord;
		}
		if (Index >= SequenceList.Num() || SequenceList[Index].PlayedState) return fRet;

		// play sound
		USoundBase* sound = GetRandomElement(SequenceList[Index].Phrase);
		if (sound) {
			if (PlayAudioComponent->IsPlaying())
				PlayAudioComponent->Stop();

			/* delete this condition to make delegate universal */
			if (	(SequenceList[Index].Phrase == EVoiceQuote::VQ_C_Ending_Put2) ||
					(SequenceList[Index].Phrase == EVoiceQuote::VO_C_Ending_RightModule) ||
					(SequenceList[Index].Phrase == EVoiceQuote::VO_C_Ending_WrondModule)) {

				OnQuotePlayingStart.Broadcast(SequenceList[Index].Phrase);

			}

			PlayAudioComponent->SetSound(sound);
			PlayAudioComponent->Play();

			//if (!SequenceList[Index].ManualSetup) SequenceList[Index].PlayedState = true;
			fRet = sound->Duration;
		}

		// set timer for branck playing
		if (SequenceList[Index].NextRecord != 0) {
			NextIndex = Index + SequenceList[Index].NextRecord;
			GetWorldTimerManager().SetTimer(hBranchPlayingTimer, this, &ACharacterVoicePreset::OnTimerPlayBranch, SequenceList[Index].ContinueInterval + fRet, false, SequenceList[Index].ContinueInterval + fRet);
		}
		NowPlayingIndex = Index;
		GetWorldTimerManager().SetTimer(hCurrentPlayingTimer, this, &ACharacterVoicePreset::OnTimerPlayCurrent, fRet, false);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Playing: " + FString::FromInt(Index) + "; the next is " + FString::FromInt(NextIndex));
	}
	return fRet;
}

void ACharacterVoicePreset::SetSequenceBranchStateByIndex(int32 Index, bool IsPlayed)
{
	if (Index < 0 || Index >= SequenceList.Num()) return;
	//UE_LOG(LogTemp, Warning, TEXT("ACharacterVoicePreset::SetSequenceBranchStateByIndex"));

	int i = Index;
	while (i < SequenceList.Num() && SequenceList[i].NextRecord > 0) {
		SequenceList[i++].PlayedState = IsPlayed;
	}
	if (i < SequenceList.Num())
		SequenceList[i].PlayedState = IsPlayed;

	NextIndex = -1;
	if (hBranchPlayingTimer.IsValid()) {
		GetWorldTimerManager().ClearTimer(hBranchPlayingTimer);
		hBranchPlayingTimer.Invalidate();
	}
}

void ACharacterVoicePreset::ResetPreset()
{
	GetWorldTimerManager().ClearTimer(hBranchPlayingTimer);
	GetWorldTimerManager().ClearTimer(hCurrentPlayingTimer);
	hBranchPlayingTimer.Invalidate();
	hCurrentPlayingTimer.Invalidate();

	if (PlayAudioComponent->IsPlaying()) PlayAudioComponent->Stop();

	for (int i = 0; i < SequenceList.Num(); i++) {
		SequenceList[i].PlayedState = false;
	}
	NowPlayingIndex = -1;
	SwitchToNextQuote = false;
}

int32 ACharacterVoicePreset::GetSequenceIndexByQuote(EVoiceQuote Quote)
{
	int index = -1;

	for (int i = 0; i < SequenceList.Num(); i++) {
		if (SequenceList[i].Phrase == Quote) {
			index = i;
			break;
		};
	}

	return index;
}

void ACharacterVoicePreset::OnTimerPlayBranch()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "OnTimerPlayBranch(NextIndex = " + FString::FromInt(NextIndex) + ");");

	hBranchPlayingTimer.Invalidate();
	PlayElementByIndex(NextIndex);
}

void ACharacterVoicePreset::OnTimerPlayCurrent()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "OnTimerPlayBranch(NowPlayingIndex = " + FString::FromInt(NowPlayingIndex) + ", NextIndex = " + FString::FromInt(NextIndex) + ");");

	if (NowPlayingIndex >= 0) {
		if (!SequenceList[NowPlayingIndex].ManualSetup) SequenceList[NowPlayingIndex].PlayedState = true;
		NowPlayingIndex = -1;
	}
	hCurrentPlayingTimer.Invalidate();

	if (SwitchToNextQuote) {
		SwitchToNextQuote = false;
		PlayElementByIndex(NextIndex);
	}
}

