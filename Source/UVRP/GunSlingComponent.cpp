// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "HandController.h"
#include "UVRPCharacter.h"
#include "UVRPGameMode.h"
#include "ThrowProjectileBase.h"
#include "ProjectileBomb.h"
#include "ProjectileTug.h"

#include "GunSlingComponent.h"

UGunSlingComponent::UGunSlingComponent()
{
	SetMobility(EComponentMobility::Type::Movable);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smAsset(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/GunSling/sm_GunSling.sm_GunSling'"));
	if (smAsset.Object != NULL) SetSkeletalMesh(smAsset.Object);
	SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
		
	static ConstructorHelpers::FObjectFinder<UAnimSequence> smAnimAsset1(TEXT("AnimSequence'/Game/FirstPerson/Objects/GunSling/anim_GunSling.anim_GunSling'"));
	if (smAnimAsset1.Object != NULL) Anim_OpenState1 = smAnimAsset1.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> sbDetachSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/pack/shield_1.shield_1'"));
	if (sbDetachSound.Object != NULL) ShieldDetachSound = sbDetachSound.Object;	

	//Class_ProjectileBomb = AProjectileBomb::StaticClass();
	Class_ProjectileSling = AProjectileTug::StaticClass();
}

void UGunSlingComponent::BeginPlay()
{
	Super::BeginPlay();

	GunState = EHeavyGunState::C_Sling;
	//ChangeGunState(EHeavyGunState::C_Bomb);

	AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(gm)) {
		FeedbackWave = gm->GetControllerFeedbackOptions(EFeedbackAction::A_SlingPull);

		if (IsValid(this->GetAttachParent())) {
			AHandController* ctrl = Cast<AHandController>(GetAttachParent()->GetOwner());
			if (IsValid(ctrl)) IsRightController = ctrl->IsRightHand();
		}
	}
	Player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetAttachmentRootActor()->GetWorld(), 0));
}

void UGunSlingComponent::ChangeGunState(EHeavyGunState NewState, bool SwapModes)
{
	/*
	if (SwapModes) {
		if (GunState == EHeavyGunState::C_Bomb)
			NewState = EHeavyGunState::C_Sling;
		else
			NewState = EHeavyGunState::C_Bomb;
	}*/

	GunState = EHeavyGunState::C_Sling;
	SpawnProjectile();

	/*
	if (GunState != NewState) {
		GunState = NewState;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Animation 1");

		SpawnProjectile();
	}
	*/
}

void UGunSlingComponent::SpawnProjectile()
{
	if (IsValid(CurrentProjectile))
		CurrentProjectile->DestroyProjectile();

	FActorSpawnParameters sp;
	const FVector loc = GetSocketLocation(SOCKET_ProjectileBase);
	const FRotator rot = GetSocketRotation(SOCKET_ProjectileBase);
	sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PlayAnimation(Anim_OpenState1, false);
	//UClass p = (GunState == EHeavyGunState::C_Bomb) ? Class_ProjectileBomb : Class_ProjectileSling;

	if (GunState == EHeavyGunState::C_Bomb) {
		//CurrentProjectile = Cast<AThrowProjectileBase>(GetWorld()->SpawnActor<AProjectileBomb>(Class_ProjectileBomb, loc, rot));
	}
	else if (GunState == EHeavyGunState::C_Sling)
		CurrentProjectile = Cast<AThrowProjectileBase>(GetWorld()->SpawnActor<AProjectileTug>(Class_ProjectileSling, loc, rot));

	if (IsValid(CurrentProjectile)) {
		CurrentProjectile->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, SOCKET_ProjectileBase);
		CurrentProjectile->SetActorRelativeLocation(FVector::ZeroVector);
		CurrentProjectile->SetActorRelativeRotation(FRotator::ZeroRotator);
		CurrentProjectile->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
		CurrentProjectile->InitializeProjectile(this);
		CurrentProjectile->SetVisibility(this->IsVisible());

		CurrentProjectile->InitParentGun(this);

		/////////////////// FEEDBACK ///////////////////
		Cast<AProjectileTug>(CurrentProjectile)->InitializeProjectileFeedback(Player, IsRightController, FeedbackWave);
		////////////////////////////////////////////////
	}
}

void UGunSlingComponent::DestroyProjectile()
{
	if (IsValid(CurrentProjectile)) {
		CurrentProjectile->Destroy();
		CurrentProjectile = nullptr;
	}
}

void UGunSlingComponent::Fire(FVector LinearVelocity)
{
	// to make things simplier
	if (!IsValid(CurrentProjectile)) {
		SpawnProjectile();
	}

	if (IsValid(CurrentProjectile)) {
		CurrentProjectile->ActivateProjectile(this, LinearVelocity, LinearVelocity);
		if (GunState == EHeavyGunState::C_Bomb) CurrentProjectile = nullptr;
	}

	if (GunState == EHeavyGunState::C_Bomb) {
		FTimerHandle hTimer;
		GetOwner()->GetWorldTimerManager().SetTimer(hTimer, this, &UGunSlingComponent::SpawnProjectile, 3.0f, false, 3.0f);
	}
}

void UGunSlingComponent::SetWithChildsVisibility(bool NewVisibility)
{
	SetVisibility(NewVisibility);
	if (IsValid(CurrentProjectile)) CurrentProjectile->SetVisibility(NewVisibility);
}

bool UGunSlingComponent::HasTug()
{
	return (GunState == EHeavyGunState::C_Sling) && IsValid(CurrentProjectile) && CurrentProjectile->IsDetached();
}

FVector UGunSlingComponent::GetTugLocation()
{
	if (IsValid(CurrentProjectile))
		return CurrentProjectile->GetActorLocation();
	else
		return FVector::ZeroVector;
}

void UGunSlingComponent::TranslatePullForce(FVector Direction, float Force)
{
	if (IsValid(CurrentProjectile)) {
		if (!IsValid(Player))
			Player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetAttachmentRootActor()->GetWorld(), 0));

		if (Force < PULL_WorkForce) {
			Player->PlayVoice(EVoiceQuote::VO_C_Gl_Harpoon_Weak);
			CurrentProjectile->DestroyProjectile();
		}
		else if (Force > PULL_MaxForce) {
			Player->PlayVoice(EVoiceQuote::VO_C_Gl_HarpoonHard);
			CurrentProjectile->DestroyProjectile();
		}
		else {

			if (CurrentProjectile->ForcePull(Direction, Force)) {
				if (IsValid(ShieldDetachSound))
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShieldDetachSound, CurrentProjectile->GetActorLocation(), 1, 1, 0, Player->GetSoundAttenuation());
			}
			CurrentProjectile = nullptr;
		}
		/////////////////// FEEDBACK ///////////////////
		if (IsValid(Player)) {
			Player->PlaySingleFeedback(EFeedbackAction::A_SlingStart, IsRightController);
		}
		////////////////////////////////////////////////
	}
}

void UGunSlingComponent::ReleaseProjectile()
{
	CurrentProjectile = nullptr;

	FTimerHandle hTimer;
	GetOwner()->GetWorldTimerManager().SetTimer(hTimer, this, &UGunSlingComponent::SpawnProjectile, 3.0f, false, 3.0f);
}
