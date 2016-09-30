// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UVRP.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "UVRPHUD.h"
#include "HandsComponent.h"
#include "HandController.h"
#include "CharacterVoicePreset.h"
#include "UVRPCharacter.h"
#include "UVRP_MySaveGame.h"
#include "UVRPPlayerController.h"
#include "PlatformBearing.h"
#include "GamePlatform.h"
#include "SaveGameOptions.h"
#include "SaveGameStatistics.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"
#include "SoldierBasic.h"
#include "Teleport.h"
#include "CoreMisc.h"
#include "SoldierBasic.h"
#include "LevelChangableInteractionActor.h"
#include "SaveObjectInterface.h"
#include "MessageWindow.h"
#include "DataJigsaw.h"
#include "DataDevice.h"

#include "UVRPGameMode.h"

AUVRPGameMode::AUVRPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/FirstPerson/Blueprints/FirstPersonCharacter.FirstPersonCharacter'"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = AUVRPPlayerController::StaticClass();
	this->PrimaryActorTick.bCanEverTick = false;

	// use our custom HUD class
	HUDClass = AUVRPHUD::StaticClass();

	bGodMode = false;
	bAutoSave = true;
	LevelLoadingParams = "";
	sr_NewSoldierName = "";
	sr_LastShotTime = -100.0f;
	sr_SoldiersCount = 0;

	GO_Max_DifficultyLevel = 3;
	GO_Max_GraphicsLevel = 4;
	CurrentFPSIndex = 0;
	GO_FloorOffset = 0.0f;

	/*
	for (int i = 0; i < CHECKFPS_TIME * CHECKFPS_PERIOD; i++)
		LastDeltaTime[i] = 0;
	*/
	ZeroMemory(&LastDeltaTime, sizeof(uint8) * CHECKFPS_TIME * CHECKFPS_PERIOD);

	/****************** GRAPHIC OPTIONS ******************/
	FGraphicsOptions opt1, opt2, opt3, opt4;
	opt1.Level = 1;
	opt1.BulletTrail = false;
	opt1.DynamicShadows = 0;
	opt1.HandsIK = false;
	opt1.PortalEffects = false;
	opt1.PortalRefraction = false;

	opt2.Level = 2;
	opt2.BulletTrail = true;
	opt2.DynamicShadows = 1;
	opt2.HandsIK = true;
	opt2.PortalEffects = true;
	opt2.PortalRefraction = true;

	opt3.Level = 3;
	opt3.BulletTrail = true;
	opt3.DynamicShadows = 2;
	opt3.HandsIK = true;
	opt3.PortalEffects = true;
	opt3.PortalRefraction = true;

	opt4.Level = 4;
	opt4.BulletTrail = true;
	opt4.DynamicShadows = 2;
	opt4.HandsIK = true;
	opt4.PortalEffects = true;
	opt4.PortalRefraction = true;

	GraphicsOptionsLevels.Add(opt1);
	GraphicsOptionsLevels.Add(opt2);
	GraphicsOptionsLevels.Add(opt3);
	GraphicsOptionsLevels.Add(opt4);
	GraphicsOptions.Level = 0;
	bAutoFPSCheck = false;
	/*****************************************************/

	FHandFeedbackSetup p1, p2, p3, p4, p5, p6;
	p1.IsLargeServo = true;	p1.DefaultIntensity = 0.6f;	p1.DefaultDuration =  0.02f; // A_PushPhysicalObject
	p2.IsLargeServo = true;	p2.DefaultIntensity = 0.3f;	p2.DefaultDuration =  0.05f; // A_CatchAndDrop
	p3.IsLargeServo = true;	p3.DefaultIntensity = 0.4f;	p3.DefaultDuration =  0.1f; // A_UnnaturalLocomotion
	p4.IsLargeServo = true;	p4.DefaultIntensity = 1.0f;	p4.DefaultDuration =  0.1f; // A_SlingStart
	p5.IsLargeServo = true;	p5.DefaultIntensity = 0.1f;	p5.DefaultDuration = -1.0f; // A_SlingPull
	p6.IsLargeServo = true;	p6.DefaultIntensity = 0.8f;	p6.DefaultDuration =  0.03f; // A_Shield

	FeedbackActions.Add(EFeedbackAction::A_PushPhysicalObject, p1);
	FeedbackActions.Add(EFeedbackAction::A_CatchAndDrop, p2);
	FeedbackActions.Add(EFeedbackAction::A_UnnaturalLocomotion, p3);
	FeedbackActions.Add(EFeedbackAction::A_SlingStart, p4);
	FeedbackActions.Add(EFeedbackAction::A_SlingPull, p5);
	FeedbackActions.Add(EFeedbackAction::A_Shield, p6);
}

void AUVRPGameMode::BeginPlay()
{
	Super::BeginPlay();

	LoadGameOptions();
	if (!UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		GEngine->Exec(GetWorld(), TEXT("stereo on"));
		//GEngine->Exec(GetWorld(), TEXT("stat fps"));
	}
}

void AUVRPGameMode::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AUVRPCharacter *player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (IsValid(player)) {
		if (!player->IsInMainMenu()) {
			player->SetupGameplayTime(false);
		}

		if (player->GetIsGameActive()) {
			SaveGame(true);
		}
	}

	SaveGameOptions();
}

// console commands ///////////////////////////////////////////////////////////////////////////////////////////////////

void AUVRPGameMode::invulnerability()
{
	bGodMode = !bGodMode;

	if (bGodMode)
		ShowMessage(FText::FromString("Godmode on"));
	else
		ShowMessage(FText::FromString("Godmode off"));
}

void AUVRPGameMode::heal()
{
	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (IsValid(player)) {
		player->Heal(1.0f);
	}	
}

void AUVRPGameMode::achreset()
{
	Ach_GameFinished = false;
	Ach_ExtendedEnding = false;
	Ach_SecretArea1 = false;
	Ach_SecretArea2 = false;
	Ach_HighDifficulty = false;
	Ach_DataModuleDecrypted = false;
	SaveGameOptions();

	Steam_ResetAchievements();
}

void AUVRPGameMode::achset(int32 number)
{
	switch (number) {
		case 0:
			UnlockAchievement(EGameAchievements::A_GameEnded);
			break;
		case 1:
			UnlockAchievement(EGameAchievements::A_ModuleDecrypted);
			break;
		case 2:
			UnlockAchievement(EGameAchievements::A_ExtendedEnding);
			break;
		case 3:
			UnlockAchievement(EGameAchievements::A_SecretArea1);
			break;
		case 4:
			UnlockAchievement(EGameAchievements::A_BothSecretAreas);
			break;
		case 5:
			UnlockAchievement(EGameAchievements::A_HighestDifficulty);
			break;
	}
}

void AUVRPGameMode::autosave(int32 mode)
{
	bAutoSave = (mode > 0);
}

void AUVRPGameMode::setplatform(int32 number)
{
	FString a, b;
	FVector loc;
	AGamePlatform *TargetPlatform = nullptr;
	AGamePlatform *CurrentPlatform = nullptr;
	AUVRPCharacter *character = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	b = (number < 10) ? "0" + FString::FromInt(number) : FString::FromInt(number);
	
	for (TActorIterator<AGamePlatform> itGP(GetWorld()); itGP; ++itGP)
	{
		a = itGP->GetName();
		//if (a.Find(b) > 0) {
		if (itGP->PlatformNameNumber == number) {
			// kill all guardians if exist
			//itGP->GuardiansData.Empty();

			TargetPlatform = *itGP;
			if (CurrentPlatform != nullptr) break;
		}
		if (IsValid(character->CurrentLocationBase) && character->CurrentLocationBase->IsA(AGamePlatform::StaticClass()) && Cast<AGamePlatform>(character->CurrentLocationBase)->PlatformNameNumber == number) {
			CurrentPlatform = *itGP;
			if (TargetPlatform != nullptr) break;
		}
	}
	if (TargetPlatform != nullptr) {
		/*
		if (CurrentPlatform != nullptr)
			loc = character->GetActorLocation() - CurrentPlatform->GetPlayerCentreLocation();
		else loc = FVector(0.0f, 85.0f, 32.0f);

		character->StopBackgroundMusicHardly();

		loc += TargetPlatform->GetPlayerCentreLocation();
		*/
		loc = TargetPlatform->GetPlayerCentreLocation();
		character->SetActorLocation(loc);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "teleported to " + TargetPlatform->GetName());
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "command execute error: " + GetWorld()->GetMapName());
	}
}

void AUVRPGameMode::giveall()
{
	AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (IsValid(player)) {
		player->SetGunLargeActive(true);
		player->SetGunSlingActive(true);
		player->SetShieldActive(true);
		player->Heal(1.0f);
	}
}

void AUVRPGameMode::OpenJigsaw()
{
	AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	int32 dmIndex = -1;

	if (IsValid(player)) {
		for (int i = 0; i < player->ControllerDataDevices.Num(); i++) {
			if (player->ControllerDataDevices[i]->UniqueNumber == 2) {
				dmIndex = i;
				break;
			}
		}

		if (dmIndex >= 0) {
			if (player->ControllerDataDevices[dmIndex]->IsProjectorEnabled()) {
				player->ControllerDataDevices[dmIndex]->SetProjectorEnabled(false);
			}

			for (int i = 0; i < 25; i++) {
				if (!player->ControllerDataDevices[dmIndex]->GetJigsawPartAvalibility(i)) {
					player->ControllerDataDevices[dmIndex]->SetJigsawPartAvalibility(i, true);
				}
			}

			ShowMessage(FText::FromString("Yellow data module can be decrypted now."));
		}
		else {
			ShowMessage(FText::FromString("You don't have the yellow data module to open it."));
		}
	}
}

void AUVRPGameMode::getplatform()
{
	const AUVRPCharacter *character = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, character->CurrentPlatformName.ToString());
}

void AUVRPGameMode::showstat()
{
	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (IsValid(player)) player->SetupGameplayTime(false);

	FString t;
	if (player->IsInTrainingLevel) {
		int32 ScoreCurr, ScoreMax;
		ScoreCurr = player->GameScore;
		ScoreMax = GP_TrainingLevelScore;
		if (ScoreMax < ScoreCurr) ScoreMax = ScoreCurr;
		t = ("Max score: " + FString::FromInt(ScoreMax) + "\n");
		t += ("Current score: " + FString::FromInt(ScoreCurr));
	}
	else {
		t = ("Soldiers killed: " + FString::FromInt(GameStat.Killed) + "\n");
		t += ("Soldiers injured: " + FString::FromInt(GameStat.Injured) + "\n");
		t += ("Player died times: " + FString::FromInt(GameStat.Died) + "\n");
		t += ("Codes Received: " + FString::FromInt(GameStat.CodesReceived) + "\n");
		t += ("Teleports Unlocked: " + FString::FromInt(GameStat.TeleportsUnlocked) + "\n");
		t += ("In-game time: " + FString::FromInt((int)(GameStat.InGameTime)) + " seconds");
	}

	TArray<FText> p;
	p.Add(FText::FromString(t));

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMessageWindow* msgbox = GetWorld()->SpawnActor<AMessageWindow>(AMessageWindow::StaticClass(), FTransform(), params);
	msgbox->SetWindowType(true);
	msgbox->SetTextData(p);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AUVRPGameMode::PrepareDataForSaving(UUVRP_MySaveGame* sgi)
{
	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!IsValid(player)) return;

	// ------------------------------------ player ------------------------------------
	sgi->PlayerHealth = player->Health;
	sgi->PlayerScore = 0;
	sgi->DifficultyLevel = GO_DifficultyLevel;
	sgi->IsDifficultyLevelChanged = GP_DifficultyLevelChanged;
	if (IsValid(player->CurrentLocationBase) && player->CurrentLocationBase->IsA(AGamePlatform::StaticClass())) {
		sgi->CurrentPlatformNumber = Cast<AGamePlatform>(player->CurrentLocationBase)->PlatformNameNumber;
	}
	sgi->IsGunLargeActive = player->GetRightController()->IsGunLargeActive;
	sgi->IsGunSlingActive = player->GetRightController()->IsGunSuperActive;
	sgi->IsGunShieldActive = player->GetRightController()->IsShieldActive;
	sgi->IsAdvancedShieldActive = player->GetRightController()->IsAdvancedShieldActive;
	sgi->CurrentLevelName = player->CurrentLevelName;
	sgi->RunningSoldiersGone = GP_RunningSoldiersGone;
	sgi->LeftHandGun = player->GetLeftController()->ControllerMode;
	sgi->RightHandGun = player->GetRightController()->ControllerMode;

	sgi->HelpMessages.Empty();
	for (int i = 0; i < player->HelpMessagesUsed.Num(); i++) {
		sgi->HelpMessages.Add(player->HelpMessagesUsed[i]);
	}

	/*
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "--- SAVING ---");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Soldiers killed: " + FString::FromInt(GameStat.Killed));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Soldiers injured: " + FString::FromInt(GameStat.Injured));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Player died times: " + FString::FromInt(GameStat.Died));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CodesReceived: " + FString::FromInt(GameStat.CodesReceived));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "TeleportsUnlocked: " + FString::FromInt(GameStat.TeleportsUnlocked));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "In-game time: " + FString::SanitizeFloat(GameStat.InGameTime) + " seconds");
	*/
	// ------------------------------------ platforms ------------------------------------
	for (TActorIterator<AGamePlatform> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		FPlatformStatus dat;
		FTransform tr;
		tr.SetLocation(ActorItr->GetActorLocation());
		tr.SetRotation(ActorItr->GetActorRotation().Quaternion());
		tr.SetScale3D(ActorItr->GetActorScale3D());

		dat.Number = ActorItr->PlatformNameNumber;
		dat.ObjectTransform = tr;
		dat.IsAttached = ActorItr->IsPassive;
		dat.IsProtected = (ActorItr->GuardiansData.Num() > 0 || ActorItr->IsDefenceActivated);
		dat.Shield1 = ActorItr->ShieldState1;
		dat.Shield2 = ActorItr->ShieldState2;
		dat.Shield3 = ActorItr->ShieldState3;
		dat.Shield4 = ActorItr->ShieldState4;
		dat.IsAvalibleForActivation = ActorItr->IsPlatformAvalibleForActivation;
		sgi->Platforms.Add(dat);
	}

	// ------------------------------------ level objects ------------------------------------
	ISaveObjectInterface* levelactor;
	//ALevelChangableInteractionActor* levelactor;
	for (TActorIterator</*AActor*/ ALevelChangableInteractionActor /* for optimisation */> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		FSimpleObjectStatus obj;
		ALevelChangableInteractionActor* p = *ActorItr;
		levelactor = Cast<ISaveObjectInterface>(p);

		if (levelactor) {
			obj.Number = levelactor->GetNumber();
			obj.IsExists = levelactor->IsExists();
			obj.OpenStarted = levelactor->IsExpansionStarted();
			obj.OpenFinished = levelactor->IsExpansionFinished();
			obj.IsActive = levelactor->IsActive();
			obj.ObjectTransform = levelactor->GetTransform();
			obj.Name = ActorItr->GetName();
		}
		sgi->LevelObjects.Add(obj);
	}

	// ------------------------------------ player data devices ------------------------------------
	for (int i = 0; i < player->ControllerDataDevices.Num(); i++) {
		FDataDeviceStatus dm;

		dm.Number = player->ControllerDataDevices[i]->UniqueNumber;
		dm.IsDecrypted = !(player->ControllerDataDevices[i]->IsEncrypted);

		FJigsawElement je;
		for (int n = 0; n < 25; n++) {
			player->ControllerDataDevices[i]->GetJigsawElement(je, n);
			dm.JigsawPieces.Add(je);
		}
		sgi->PlayerDataModules.Add(dm);
	}

	// ------------------------------------ voice ------------------------------------
	for (int i = 0; i < player->VoicePreset->SequenceList.Num(); i++) {
		sgi->VoicePlayingState.Add(player->VoicePreset->SequenceList[i].PlayedState);
	}
}

void AUVRPGameMode::LoadSavedDataToLevel(UUVRP_MySaveGame* sgi)
{
	AUVRPCharacter* player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	AGamePlatform* playerplatform = nullptr;
	if (!IsValid(player)) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "LoadSavedDataToLevel(" + FString::FromInt(sgi->CurrentPlatformNumber) + ")");

	if (sgi->PlayerHealth < 0.3f) {
		player->Health = 0.0f;
		player->Heal(0.3f);
	} else player->Health = sgi->PlayerHealth;

	SetDifficultyLevel(sgi->DifficultyLevel);
	GP_DifficultyLevelChanged = sgi->IsDifficultyLevelChanged;

	player->SetGunLargeActive(sgi->IsGunLargeActive);
	player->SetGunSlingActive(sgi->IsGunSlingActive);
	player->SetShieldActive(sgi->IsGunShieldActive);
	player->SetAdvancedShieldActive(sgi->IsAdvancedShieldActive);
	player->CurrentLevelName = sgi->CurrentLevelName;
	player->GetLeftController()->UpdateControllerMode(sgi->LeftHandGun);
	player->GetRightController()->UpdateControllerMode(sgi->RightHandGun);
	GP_RunningSoldiersGone = sgi->RunningSoldiersGone;

	player->HelpMessagesUsed.Empty();
	for (int i = 0; i < sgi->HelpMessages.Num(); i++) {
		player->HelpMessagesUsed.Add(sgi->HelpMessages[i]);
	}

	// ------------------------------------ kill occasional soldiers ------------------------------------
	for (TActorIterator<ASoldierBasic> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		//ActorItr->GetMesh()->GetAnimInstance()
		ActorItr->GetController()->Destroy();
		if (IsValid(*ActorItr)) ActorItr->GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
		if (IsValid(*ActorItr)) ActorItr->Destroy();
	}

	// ------------------------------------ platforms ------------------------------------
	for (TActorIterator<AGamePlatform> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		for (int i = 0; i < sgi->Platforms.Num(); i++) {
			if (sgi->Platforms[i].Number == ActorItr->PlatformNameNumber) {
				if (ActorItr->IsPassive && !sgi->Platforms[i].IsAttached) {
					if (IsValid(ActorItr->ParentMechanism)) {
						Cast<APlatformBearing>(ActorItr->ParentMechanism)->DetachPlatformActor();
						ActorItr->DetachFromBearingMech(true);
					}
				}				
				ActorItr->IsPassive = sgi->Platforms[i].IsAttached;
				if (ActorItr->IsRootComponentMovable()) {
					ActorItr->SetActorLocation(sgi->Platforms[i].ObjectTransform.GetLocation());
					ActorItr->SetActorRotation(sgi->Platforms[i].ObjectTransform.GetRotation().Rotator());
				}
				if (!sgi->Platforms[i].IsProtected) {
					ActorItr->GuardiansData.Empty();
					ActorItr->IsPlatformAvalibleForActivation = sgi->Platforms[i].IsAvalibleForActivation;
					ActorItr->SetAvalibleForActivation();
				}

				ActorItr->ShieldState1 = sgi->Platforms[i].Shield1;
				ActorItr->ShieldState2 = sgi->Platforms[i].Shield2;
				ActorItr->ShieldState3 = sgi->Platforms[i].Shield3;
				ActorItr->ShieldState4 = sgi->Platforms[i].Shield4;
				ActorItr->UpdatePlatformShields();

				break;
			}
		}

		if (ActorItr->PlatformNameNumber == sgi->CurrentPlatformNumber)
			playerplatform = *ActorItr;
	}

	// ------------------------------------ level objects ------------------------------------
	ISaveObjectInterface* levelactor;
	//ALevelChangableInteractionActor* levelactor;
	for (TActorIterator</*AActor*/ ALevelChangableInteractionActor /* for optimisation */> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		bool ActorExists = false;
		FSimpleObjectStatus obj;
		ALevelChangableInteractionActor* p = *ActorItr;
		levelactor = Cast<ISaveObjectInterface>(p);

		for (int i = 0; i < sgi->LevelObjects.Num(); i++) {
			if (sgi->LevelObjects[i].Number == levelactor->GetNumber()) {
				/*
				if (sgi->LevelObjects[i].Number == 401 || sgi->LevelObjects[i].Number == 402 || sgi->LevelObjects[i].Number == 403 || sgi->LevelObjects[i].Number == 404) {
					FString started = sgi->LevelObjects[i].OpenStarted ? "true" : "false";
					FString finished = sgi->LevelObjects[i].OpenFinished ? "true" : "false";
					FString exists = sgi->LevelObjects[i].IsExists ? "true" : "false";
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::FromInt(sgi->LevelObjects[i].Number) + " IsExists=" + exists + " Started=" + started + " Finished="+ finished);
				}
				*/
				if (sgi->LevelObjects[i].IsExists) {
					ActorExists = true;

					levelactor->SetTransform(sgi->LevelObjects[i].ObjectTransform);
					if (sgi->LevelObjects[i].OpenFinished) {
						levelactor->SetExpansionFinished();
					}
					else if (sgi->LevelObjects[i].OpenStarted) {
						levelactor->SetExpansionStarted();
					}
					if (sgi->LevelObjects[i].IsActive) levelactor->SetActive();
				}
				break;
			}
		}

		if (!ActorExists) {
			//ActorItr->Destroy();
			levelactor->DestroyActor();
		}
	}
	// ------------------------------------ player data devices ------------------------------------
	for (TActorIterator<ADataDevice> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		for (int i = 0; i < sgi->PlayerDataModules.Num(); i++) {
			if (ActorItr->UniqueNumber != 1 /* ignore main device */ && sgi->PlayerDataModules[i].Number == ActorItr->UniqueNumber) {
				player->TakeDataDeviceAuto(*ActorItr);
				ActorItr->IsEncrypted = !sgi->PlayerDataModules[i].IsDecrypted;

				for (int n = 0; n < 25; n++) {
					ActorItr->SetJigsawElement(sgi->PlayerDataModules[i].JigsawPieces[n], n);
				}

				break;
			}
		}
	}

	// ------------------------------------ voice ------------------------------------
	for (int i = 0; i < player->VoicePreset->SequenceList.Num(); i++) {
		if (i < sgi->VoicePlayingState.Num()) {
			player->VoicePreset->SequenceList[i].PlayedState = sgi->VoicePlayingState[i];
		}
		else break;
	}

	// ------------------------------------ player location ------------------------------------
	if (sgi->CurrentLevelName == TEXT("main") && IsValid(playerplatform)) {
		FVector loc = playerplatform->GetPlayerCentreLocation();
		player->SetActorLocation(loc);

		IsLoadingGame = false;
		if (IsValid(playerplatform)) player->OnPlatformBeginManually(playerplatform);	
		player->SetIsGameActive(true);
	}
}

void AUVRPGameMode::LoadGameStatistics()
{
	USaveGameStatistics* sgs = Cast<USaveGameStatistics>(UGameplayStatics::CreateSaveGameObject(USaveGameStatistics::StaticClass()));
	sgs = Cast<USaveGameStatistics>(UGameplayStatics::LoadGameFromSlot("gamestat1", 0));
	GameStat.CodesReceived = sgs->CodesReceived;
	GameStat.Died = sgs->Died;
	GameStat.InGameTime = sgs->InGameTime;
	GameStat.Injured = sgs->Injured;
	GameStat.Killed = sgs->Killed;
	GameStat.TeleportsUnlocked = sgs->TeleportsUnlocked;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "LoadGameStatistics(InGameTime = " + FString::SanitizeFloat(GameStat.InGameTime) + ")");
}

void AUVRPGameMode::SaveGameStatistics()
{
	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (IsValid(player)) player->SetupGameplayTime(false);
	if (IsLoadingGame) return;
	if (player->Health > 0.0f && !player->GetIsGameActive()) return;

	USaveGameStatistics* sgs = Cast<USaveGameStatistics>(UGameplayStatics::CreateSaveGameObject(USaveGameStatistics::StaticClass()));
	sgs->CodesReceived = GameStat.CodesReceived;
	sgs->Died = GameStat.Died;
	sgs->InGameTime = GameStat.InGameTime;
	sgs->Injured = GameStat.Injured;
	sgs->Killed = GameStat.Killed;
	sgs->TeleportsUnlocked = GameStat.TeleportsUnlocked;

	UGameplayStatics::SaveGameToSlot(sgs, "gamestat1", 0);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "SaveGameStatistics(InGameTime = " + FString::SanitizeFloat(GameStat.InGameTime) + ")");
}

AMessageWindow* AUVRPGameMode::ShowMessage(FText Text, float LifeSpan)
{
	TArray<FText> p;
	p.Add(Text);

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMessageWindow* msgbox = GetWorld()->SpawnActor<AMessageWindow>(AMessageWindow::StaticClass(), FTransform(), params);
	msgbox->SetWindowType(false, LifeSpan);
	msgbox->SetTextData(p);

	return msgbox;
}

AMessageWindow* AUVRPGameMode::ShowDataWindow(int32 DataModuleNumber, FName FileName, AActor* Parent, FTransform RelativeTransform)
{
	AMessageWindow* msgbox = nullptr;
	TArray<FText> p = ReadTextFromDataTable(DataModuleNumber, FileName);

	if (p.Num() > 0) {
		FString tmp;
		for (int i = 0; i < p.Num(); i++) {
			tmp = p[i].ToString().Replace(TEXT("|"), TEXT("\n"), ESearchCase::Type::IgnoreCase);
			p[i] = FText::FromString(tmp);
		}

		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		msgbox = GetWorld()->SpawnActor<AMessageWindow>(AMessageWindow::StaticClass(), RelativeTransform, params);
		if (IsValid(Parent)) {
			msgbox->AttachToComponent(Parent->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			msgbox->SetActorRelativeLocation(RelativeTransform.GetLocation());
			msgbox->SetActorRelativeRotation(RelativeTransform.GetRotation());
		}
		msgbox->SetWindowType(true);
		msgbox->SetTextData(p);
	}

	return msgbox;
}

FHandFeedbackSetup AUVRPGameMode::GetControllerFeedbackOptions(EFeedbackAction Action)
{
	/*
	int index = -1;
	for (int i = 0; i < FeedbackActions.Num(); i++) {
		if (FeedbackActions[i].Action == Action) {
			index = i; break;
		}
	}

	if (index == -1)
		return FHandFeedbackSetup();
	else
		return FeedbackActions[index];
	*/
	return FeedbackActions[Action];
}

void AUVRPGameMode::LoadTextFile(FString FileName, FString& Buffer)
{
	FileName = FPaths::GameConfigDir() + FileName + ".datamodule";
	bool AllowOverwriting = true;	

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FileName);
	//IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile;

	if (AllowOverwriting /*|| !PlatformFile::FileExists(*FileName)*/)
	{
		FFileHelper::SaveStringToFile(Buffer, *FileName);
	}
}

void AUVRPGameMode::SaveGameOptions()
{
	USaveGameOptions* SaveOptionsInstance = Cast<USaveGameOptions>(UGameplayStatics::CreateSaveGameObject(USaveGameOptions::StaticClass()));
	SaveOptionsInstance->bShowHands = GO_ShowHands;
	SaveOptionsInstance->fSoundVolume = GO_SoundVolume;
	SaveOptionsInstance->fMusicVolume = GO_MusicVolume;
	SaveOptionsInstance->fVoiceVolume = GO_VoiceVolume;
	SaveOptionsInstance->nDifficultyLevel = GO_DifficultyLevel;
	SaveOptionsInstance->nGraphicsLevel = GO_GraphicsLevel;
	SaveOptionsInstance->bAchevementGameFinished = Ach_GameFinished;
	SaveOptionsInstance->bAchevementExtendedEnding = Ach_ExtendedEnding;
	SaveOptionsInstance->bAchevementSecretArea1 = Ach_SecretArea1;
	SaveOptionsInstance->bAchevementSecretArea2 = Ach_SecretArea2;
	SaveOptionsInstance->bAchevementHighDifficulty = Ach_HighDifficulty;
	SaveOptionsInstance->bAchevementDataModuleDecrypted = Ach_DataModuleDecrypted;
	SaveOptionsInstance->nTrainingMaxScore = GP_TrainingLevelScore;
	SaveOptionsInstance->fEndingMusicPlayingTime = GP_EndingMusicPlayingTime;
	SaveOptionsInstance->bTutorialWasRead = GP_TutorialWasRead;
	SaveOptionsInstance->bAutoDecreaseGraphicsLevel = GO_AutoDecreaseGraphicsLevel;
	SaveOptionsInstance->bGameplayTutorial = GO_GameplayTutorial;
	SaveOptionsInstance->fFloorOffset = GO_FloorOffset;

	UGameplayStatics::SaveGameToSlot(SaveOptionsInstance, SaveOptionsInstance->SaveSlotName, SaveOptionsInstance->UserIndex);
}

void AUVRPGameMode::LoadGameOptions(bool ApplyOptions)
{
	USaveGameOptions* LoadOptionsInstance = Cast<USaveGameOptions>(UGameplayStatics::CreateSaveGameObject(USaveGameOptions::StaticClass()));
	LoadOptionsInstance = Cast<USaveGameOptions>(UGameplayStatics::LoadGameFromSlot(LoadOptionsInstance->SaveSlotName, LoadOptionsInstance->UserIndex));

	GO_DifficultyLevel = 100;
	if (IsValid(LoadOptionsInstance)) {
		GO_ShowHands = LoadOptionsInstance->bShowHands;
		GO_SoundVolume = LoadOptionsInstance->fSoundVolume;
		GO_MusicVolume = LoadOptionsInstance->fMusicVolume;
		GO_VoiceVolume = LoadOptionsInstance->fVoiceVolume;
		SetDifficultyLevel(LoadOptionsInstance->nDifficultyLevel);
		GO_GraphicsLevel = LoadOptionsInstance->nGraphicsLevel;
		GO_AutoDecreaseGraphicsLevel = LoadOptionsInstance->bAutoDecreaseGraphicsLevel;
		GO_GameplayTutorial = LoadOptionsInstance->bGameplayTutorial;

		Ach_GameFinished = LoadOptionsInstance->bAchevementGameFinished;
		Ach_ExtendedEnding = LoadOptionsInstance->bAchevementExtendedEnding;
		Ach_SecretArea1 = LoadOptionsInstance->bAchevementSecretArea1;
		Ach_SecretArea2 = LoadOptionsInstance->bAchevementSecretArea2;
		Ach_HighDifficulty = LoadOptionsInstance->bAchevementHighDifficulty;
		Ach_DataModuleDecrypted = LoadOptionsInstance->bAchevementDataModuleDecrypted;

		GP_TrainingLevelScore = LoadOptionsInstance->nTrainingMaxScore;
		GP_EndingMusicPlayingTime = LoadOptionsInstance->fEndingMusicPlayingTime;
		GP_TutorialWasRead = LoadOptionsInstance->bTutorialWasRead;
		GO_FloorOffset = LoadOptionsInstance->fFloorOffset;
	}
	
	if (!IsValid(LoadOptionsInstance) || GO_GraphicsLevel == 0) {
		SetGraphicsLevel(3, true);
		SetDifficultyLevel(1);
		GO_SoundVolume = 0.45f;
		GO_VoiceVolume = 1.0f;
		GO_MusicVolume = 0.4f;
		GP_TrainingLevelScore = 0;
		GP_EndingMusicPlayingTime = 0.0f;
		GO_FloorOffset = 0.0f;
		GO_ShowHands = false;
		GO_AutoDecreaseGraphicsLevel = false;
		Ach_GameFinished = Ach_ExtendedEnding = Ach_SecretArea1 = Ach_SecretArea2 = Ach_HighDifficulty = false;
		GO_GameplayTutorial = true;
	}
	if (GO_GraphicsLevel < 1 || GO_GraphicsLevel > GO_Max_GraphicsLevel) SetGraphicsLevel(2, true);
	bAutoFPSCheck = GO_AutoDecreaseGraphicsLevel;

	if (ApplyOptions) {
		SetAudioVolume(VOLUME_SC_Ambience, GO_SoundVolume);
		SetAudioVolume(VOLUME_SC_Music, GO_MusicVolume);
		SetAudioVolume(VOLUME_SC_Sound, GO_SoundVolume);
		SetAudioVolume(VOLUME_SC_Voice, GO_VoiceVolume);

		//GO_AutoDecreaseGraphicsLevel = false;
		bAutoFPSCheck = GO_AutoDecreaseGraphicsLevel;
		SetGraphicsLevel(GO_GraphicsLevel, true);
		SetAutoFPSCheck(GO_AutoDecreaseGraphicsLevel);
	}
}

void AUVRPGameMode::SetAudioVolume(FString ClassName, float VolumeMultiplier)
{
	FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();
	if (!AudioDevice) return;

	for (auto i = AudioDevice->SoundClasses.CreateIterator(); i; ++i) {
		USoundClass* SoundClass = i.Key();
		FString SoundClassName;

		if (SoundClass->GetFullName().Split(L".", nullptr, &SoundClassName, ESearchCase::CaseSensitive) && SoundClassName.Equals(ClassName)) {
			SoundClass->Properties.Volume = VolumeMultiplier;
			break;
		}
	}
}

void AUVRPGameMode::SetDifficultyLevel(uint8 NewLevel)
{
	if (GO_DifficultyLevel != NewLevel) {
		GO_DifficultyLevel = NewLevel;
		GP_DifficultyLevelChanged = true;

		float PlayerDamageMultiplier = 1.0f;
		switch (NewLevel) {
			case 1:
				PlayerDamageMultiplier = 0.5f; SoldierDamageMultiplier = 1.3f; TeleportDecryptTimeout = 24.0f; break;
			case 2:
				PlayerDamageMultiplier = 1.2f; SoldierDamageMultiplier = 0.7f; TeleportDecryptTimeout = 17.0f; break;
			case 3:
				PlayerDamageMultiplier = 1.5f; SoldierDamageMultiplier = 0.5f; TeleportDecryptTimeout = 14.0f; break;
		}

		AUVRPCharacter* player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (IsValid(player)) player->DamageMultiplier = PlayerDamageMultiplier;

		for (TActorIterator<ASoldierBasic> itBS(GetWorld()); itBS; ++itBS)
			itBS->DamageMultiplier = SoldierDamageMultiplier;

		for (TActorIterator<ATeleport> itT(GetWorld()); itT; ++itT)
			itT->TeleportUnlockInterval = TeleportDecryptTimeout;
	}
}

void AUVRPGameMode::SetGraphicsLevel(uint8 NewLevel, bool ForceUpdate)
{
	if (GraphicsOptionsLevels.Num() == 3) {
		FGraphicsOptions opt4;
		opt4.Level = 4;
		opt4.BulletTrail = true;
		opt4.DynamicShadows = 2;
		opt4.HandsIK = true;
		opt4.PortalEffects = true;
		opt4.PortalRefraction = true;
		GraphicsOptionsLevels.Add(opt4);
	}
	//ShowMessage(FText::FromString("GraphicsOptionsLevels.Num() == " + FString::FromInt(GraphicsOptionsLevels.Num()) + "\nGO_GraphicsLevel == " + FString::FromInt(GO_GraphicsLevel)));

	if (NewLevel > GraphicsOptionsLevels.Num()) {
		NewLevel = GraphicsOptionsLevels.Num();
	}
	if (!ForceUpdate && NewLevel == GO_GraphicsLevel) return;

	GraphicsOptions = GraphicsOptionsLevels[NewLevel - 1];
	GO_GraphicsLevel = NewLevel;

	// 1+2. Portal Effects and IK Hands
	AUVRPCharacter* player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (IsValid(player)) {
		// turn on in menu

		// no IK hands (for now)
		//player->UpdateHandsView(GO_ShowHands);

		player->SetControllersType(false /*GraphicsOptions.HandsIK*/);
		// portal camera		
		if (player->IsPortalVisible) player->OnEndTeleportPlatform();
		player->CanShowTeleportPortal = false; //GraphicsOptions.PortalEffects;
		
		// tick
		player->SetActorTickEnabled(false); // GraphicsOptions.PortalEffects || GraphicsOptions.HandsIK);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Player Is not ready");
	}

	//r.screenpercentage=130
	APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PController) //PController->ConsoleCommand(TEXT("r.ScreenPercentage 130"), true);		
	{
		//PController->ConsoleCommand(TEXT("stat fps"), true);
		switch (NewLevel) {
			case 1:
				//GetWorld()->Exec(GetWorld(), TEXT("r.ScreenPercentage 80"));
				PController->ConsoleCommand(TEXT("r.ScreenPercentage 115"), true);
				break;
			case 2:
				PController->ConsoleCommand(TEXT("r.ScreenPercentage 130"), true);
				//GetWorld()->Exec(GetWorld(), TEXT("r.ScreenPercentage 130"));
				break;
			case 3:
				PController->ConsoleCommand(TEXT("r.ScreenPercentage 150"), true);
				//GetWorld()->Exec(GetWorld(), TEXT("r.ScreenPercentage 150"));
				break;
			case 4:
				PController->ConsoleCommand(TEXT("r.ScreenPercentage 190"), true);
		}
	}

	// 3. Teleport Field Refraction
	for (TActorIterator<ATeleport> itT(GetWorld()); itT; ++itT)
		itT->ShowFieldEffects = GraphicsOptions.PortalRefraction;

	// 4. Bullet Trails
	// TActorIterator <ASoldierBasic> t;
	// within GunComponent

	// 5. Dynamic Shadows
	/*
	for (TActorIterator<APointLight> itLight(GetWorld()); itLight; ++itLight) {
		if (itLight->IsRootComponentStationary()) {
			if (itLight->ActorHasTag("DynamicShadowsSetup_1"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 1);
			if (itLight->ActorHasTag("DynamicShadowsSetup_2"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 2);
			if (itLight->ActorHasTag("DynamicShadowsSetup_3"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 3);
		}
	}
	for (TActorIterator<ASpotLight> itLight(GetWorld()); itLight; ++itLight) {
		if (itLight->IsRootComponentStationary()) {
			if (itLight->ActorHasTag("DynamicShadowsSetup_1"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 1);
			if (itLight->ActorHasTag("DynamicShadowsSetup_2"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 2);
			if (itLight->ActorHasTag("DynamicShadowsSetup_3"))
				itLight->SetCastShadows(GraphicsOptions.DynamicShadows >= 3);
		}
	}
	*/
}

void AUVRPGameMode::SetAutoFPSCheck(bool IsEnabled)
{
	if (IsEnabled && !hTimerCheckFPS.IsValid() && GraphicsOptions.Level > 1) {
		GetWorldTimerManager().SetTimer(hTimerCheckFPS, this, &AUVRPGameMode::Timer_CheckFPS, 1.0f / (float)CHECKFPS_PERIOD, true);
	}
	else if (!IsEnabled && hTimerCheckFPS.IsValid()) {
		GetWorldTimerManager().ClearTimer(hTimerCheckFPS);
		CurrentFPSIndex = 0;
	}
	GO_AutoDecreaseGraphicsLevel = IsEnabled;
	bAutoFPSCheck = IsEnabled;
}

void AUVRPGameMode::UnlockAchievement(EGameAchievements Achievement)
{
	bool PrevValue = false;
	switch (Achievement) {
		case EGameAchievements::A_GameEnded:
			PrevValue = Ach_GameFinished;
			Ach_GameFinished = true;
			break;
		case EGameAchievements::A_ModuleDecrypted:
			PrevValue = Ach_DataModuleDecrypted;
			Ach_DataModuleDecrypted = true;
			break;
		case EGameAchievements::A_ExtendedEnding:
			PrevValue = Ach_ExtendedEnding;
			Ach_ExtendedEnding = true;
			break;
		case EGameAchievements::A_SecretArea1:
			PrevValue = Ach_SecretArea1;
			Ach_SecretArea1 = true;
			break;
		case EGameAchievements::A_SecretArea2:
			PrevValue = Ach_SecretArea2;
			Ach_SecretArea2 = true;
			break;
		case EGameAchievements::A_HighestDifficulty:
			PrevValue = Ach_HighDifficulty;
			Ach_HighDifficulty = true;
			break;
	}

	if (Achievement == EGameAchievements::A_GameEnded || Achievement == EGameAchievements::A_ExtendedEnding) {
		AUVRPCharacter* player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (IsValid(player))
			player->SetupGameplayTime(false);
	}

	if (!PrevValue) {
		if (Achievement == EGameAchievements::A_SecretArea1 || Achievement == EGameAchievements::A_SecretArea2) {
			if (Ach_SecretArea1 && Ach_SecretArea2) {
				Achievement = EGameAchievements::A_BothSecretAreas;
			}
		}

		Steam_SaveAchievement(Achievement);
	}
}

bool AUVRPGameMode::IsAchievementUnlocked(EGameAchievements Achievement)
{
	bool bRet = false;

	switch (Achievement) {
		case EGameAchievements::A_GameEnded:
			bRet = Ach_GameFinished;
			break;
		case EGameAchievements::A_ModuleDecrypted:
			bRet = Ach_DataModuleDecrypted;
			break;
		case EGameAchievements::A_ExtendedEnding:
			bRet = Ach_ExtendedEnding;
			break;
		case EGameAchievements::A_SecretArea1:
		case EGameAchievements::A_SecretArea2:
			bRet = (Ach_SecretArea1 || Ach_SecretArea2);
			break;
		case EGameAchievements::A_HighestDifficulty:
			bRet = Ach_HighDifficulty;
			break;
		case EGameAchievements::A_BothSecretAreas:
			bRet = (Ach_SecretArea1 && Ach_SecretArea2);
			break;
	}

	return bRet;
}

void AUVRPGameMode::StartNewGame(bool Restart)
{
	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!IsValid(player)) return;

	/*
	if (Restart) {
		FLatentActionInfo opt;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("intro"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("main"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("upper"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("trainingroom"), opt);
		UGameplayStatics::LoadStreamLevel(GetWorld(), TEXT("main"), false, true, opt);
	}
	*/

	GP_DifficultyLevelChanged = false;
	GP_RunningSoldiersGone = false;
	GameStat.Killed = GameStat.Injured = GameStat.Died = GameStat.TeleportsUnlocked = GameStat.CodesReceived = 0;
	GameStat.InGameTime = 0.0f;

	player->Health = 1.0f;
	player->Damage(0.3f);
	player->AI_VoicePreset->ResetPreset();

	player->SetGunSmallActive(true);
	player->SetGunLargeActive(false);
	player->SetGunSlingActive(false);
	player->SetShieldActive(false);
	player->SetAdvancedShieldActive(true);

	// reset data devices to match current difficulty level
	for (TActorIterator<ADataDevice> DM_Itr(GetWorld()); DM_Itr; ++DM_Itr) {		
		if (DM_Itr->IsEncrypted && DM_Itr->IsAvalibleForUnlock) {
			ADataJigsaw::ResetJigsaw(*DM_Itr, GO_DifficultyLevel);
		}
	}

	if (Restart) {
		player->GetLeftController()->PutStaticMesh();
		player->GetRightController()->PutStaticMesh();

		for (int i = 1; i < player->ControllerDataDevices.Num(); i++) {
			if (player->ControllerDataDevices[i]->UniqueNumber == DATAMODULE_YELLOW) {
				for (int n = 0; i < 25; n++) {
					player->ControllerDataDevices[i]->SetJigsawPartAvalibility(n, false);
					player->ControllerDataDevices[i]->SetJigsawState(n, 0);
				}
				player->ControllerDataDevices[i]->DetachFormController();
				player->PutDataDevice(i);
				break;
			}
		}
	}
	player->ResetGameplayTime();
	player->SetIsGameActive(true);
}

bool AUVRPGameMode::IsSaveGameExists()
{
	return UGameplayStatics::DoesSaveGameExist(TEXT("game1"), 0);
}

void AUVRPGameMode::LoadGame()
{
	if (IsSaveGameExists()) {
		/*
		FLatentActionInfo opt;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("intro"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("main"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("upper"), opt);
		UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("trainingroom"), opt);
		UGameplayStatics::FlushLevelStreaming(GetWorld());

		UGameplayStatics::LoadStreamLevel(GetWorld(), TEXT("main"), true, false, opt);
		UGameplayStatics::GetStreamingLevel(GetWorld(), TEXT("game"))->BroadcastLevelLoadedStatus(GetWorld(), TEXT("main"), true);

		UGameplayStatics::FlushLevelStreaming(GetWorld());
		*/

		IsLoadingGame = true;

		AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (IsValid(player)) player->TeleportToLevel("main");

		GetWorldTimerManager().SetTimer(hTimerLoadLevel, this, &AUVRPGameMode::Timer_LoadLevel, 0.5f, false, 0.5f);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Save game file is corrupted");
	}
}

void AUVRPGameMode::Timer_LoadLevel()
{
	GetWorldTimerManager().ClearTimer(hTimerLoadLevel);
	hTimerLoadLevel.Invalidate();

	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	/*
	if (IsValid(player)) player->TeleportToLevel("main");
	*/

	UUVRP_MySaveGame* LoadGameInstance = Cast<UUVRP_MySaveGame>(UGameplayStatics::CreateSaveGameObject(UUVRP_MySaveGame::StaticClass()));
	LoadGameInstance = Cast<UUVRP_MySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("game1"), 0));
	if (IsValid(LoadGameInstance)) {
		LoadSavedDataToLevel(LoadGameInstance);
	}

	// update audio settings if level was reloaded
	SetAudioVolume(VOLUME_SC_Ambience, GO_SoundVolume);
	SetAudioVolume(VOLUME_SC_Music, GO_MusicVolume);
	SetAudioVolume(VOLUME_SC_Sound, GO_SoundVolume);
	SetAudioVolume(VOLUME_SC_Voice, GO_VoiceVolume);

	LoadGameStatistics();
	player->SetupGameplayTime(true);

	OnGameLoadingFinished();
}

void AUVRPGameMode::SaveGame(bool ForceSave)
{
	if (IsLoadingGame) return;

	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!IsValid(player)) return;
	if (player->CurrentLevelName != TEXT("main") && !ForceSave) return;

	SaveGameStatistics();

	UUVRP_MySaveGame* SaveGameInstance = Cast<UUVRP_MySaveGame>(UGameplayStatics::CreateSaveGameObject(UUVRP_MySaveGame::StaticClass()));
	PrepareDataForSaving(SaveGameInstance);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("game1"), 0);
}

void AUVRPGameMode::AutoSaveGame(uint8 ParamType, int32 ParamValue)
{
	if (ParamType == 1) {
		if (LastGamePlatform == ParamValue)
			return;
		else
			LastGamePlatform = ParamValue;
	}
	SaveGame();
}

void AUVRPGameMode::Timer_CheckFPS()
{
	const float target_dt = 1.0f / 88.0f;
	float dt = GetWorld()->GetDeltaSeconds();
	LastDeltaTime[CurrentFPSIndex] = (dt > target_dt ? 1 : 0);

	int summ = 0;
	for (int i = 0; i < CHECKFPS_TIME * CHECKFPS_PERIOD; i++) {
		summ += LastDeltaTime[i];
	}
	if (summ > CHECKFPS_TIME+2) {
		if (GraphicsOptions.Level > 1) {
			SetGraphicsLevel(GraphicsOptions.Level - 1);
			ZeroMemory(&LastDeltaTime, sizeof(uint8) * CHECKFPS_TIME * CHECKFPS_PERIOD);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Graphics Quality Levels decreased to  " + FString::FromInt(GraphicsOptions.Level));
		}
		else
			SetAutoFPSCheck(false);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::FromInt(CurrentFPSIndex) + " - " + FString::FromInt(summ) + " - " + FString::SanitizeFloat(dt));

	CurrentFPSIndex++;
	if (CurrentFPSIndex >= CHECKFPS_TIME * CHECKFPS_PERIOD) CurrentFPSIndex = 0;
}
