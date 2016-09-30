// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Bomb.h"
#include "Engine.h"
#include "UVRPCharacter.h"
#include "SoldierBasic.h"
#include "HandController.h"

ABomb::ABomb()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smAsset(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/GunSling/sm_Bomb.sm_Bomb'"));
	if (smAsset.Object != NULL) this->GetSkeletalMeshComponent()->SetSkeletalMesh(smAsset.Object);
	GetSkeletalMeshComponent()->SetSimulatePhysics(false);
	GetSkeletalMeshComponent()->BodyInstance.SetCollisionProfileName("Projectile");
	GetSkeletalMeshComponent()->OnComponentHit.AddDynamic(this, &ABomb::OnHit);
	GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> smAnimAsset1(TEXT("AnimSequence'/Game/FirstPerson/Objects/GunSling/anim_Bomb_Up.anim_Bomb_Up'"));
	if (smAnimAsset1.Object != NULL) AnimOpen = smAnimAsset1.Object;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> smAnimAsset2(TEXT("AnimSequence'/Game/FirstPerson/Objects/GunSling/anim_Bomb_Down.anim_Bomb_Down'"));
	if (smAnimAsset2.Object != NULL) AnimClose = smAnimAsset2.Object;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(GetSkeletalMeshComponent());
	CollisionComp->SetRelativeLocation(FVector(0.0f, 0.0f, 6.3f));
	CollisionComp->InitSphereRadius(7.0f);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	CollisionComp->SetNotifyRigidBodyCollision(false);
	CollisionComp->bGenerateOverlapEvents = false;
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->OnComponentHit.AddDynamic(this, &ABomb::OnHit);

	TriggerComp = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComp"));
	TriggerComp->SetupAttachment(GetSkeletalMeshComponent());
	TriggerComp->InitSphereRadius(6.9f);
	TriggerComp->SetRelativeLocation(FVector(0.0f, 0.0f, 6.3f));
	TriggerComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	TriggerComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	TriggerComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	TriggerComp->SetNotifyRigidBodyCollision(false);
	TriggerComp->bGenerateOverlapEvents = true;
	TriggerComp->CanCharacterStepUpOn = ECB_No;
	TriggerComp->SetSimulatePhysics(false);
	TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ABomb::External_OnBeginOverlap);
	//TriggerComp->OnComponentHit.AddDynamic(this, &ABomb::OnHit1);

	ExternalCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExternalCollisionComp"));
	ExternalCollisionComp->SetupAttachment(CollisionComp);
	ExternalCollisionComp->SetRelativeLocation(FVector::ZeroVector);
	ExternalCollisionComp->InitSphereRadius(BOMB_MAX_DAMAGE_RADIUS);
	ExternalCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ExternalCollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ExternalCollisionComp->CanCharacterStepUpOn = ECB_No;
	ExternalCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExternalCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABomb::Trigger_OnBeginOverlap);
	ExternalCollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABomb::Trigger_OnEndOverlap);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> psExAsset(TEXT("ParticleSystem'/Game/FirstPerson/Effects/P_Pixel_Explosion_Player.P_Pixel_Explosion_Player'"));
	if (psExAsset.Object != NULL) ExplosionEffect = psExAsset.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> sbFireSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/pack/explosion_1.explosion_1'"));
	if (sbFireSound.Object != NULL) FireSound = sbFireSound.Object;

	bState = false;
	bIsAttached = false;
	IsPassive = true;
	DisableSoldierArray = false;
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
	mti_Light = this->GetSkeletalMeshComponent()->CreateDynamicMaterialInstance(1);
	if (IsValid(mti_Light)) mti_Light->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor::Green);
}

void ABomb::OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float MaxDamage = 3.4f;
	float Damage;

	if (IsPassive || OtherActor->IsA(AUVRPCharacter::StaticClass()) || OtherComp->ComponentHasTag(TEXT("HeadCollider")) || OtherActor->IsA(AHandController::StaticClass()) || OtherActor->IsA(ABomb::StaticClass())) return;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "BombHit to: " + OtherActor->GetName() + "->" + OtherComp->GetName());

	if (bState) {
		AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		// don't explode near the player
		if ((player->GetHeadLocation() - GetActorLocation()).Size() <= 120.0f) return;

		DisableSoldierArray = true;
		ExternalCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // don't kill just-borning baby soldiers!

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), NormalImpulse.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation(), 1.0f, 1.0f, 0.0f, player->GetSoundAttenuation());
		GetSkeletalMeshComponent()->SetSimulatePhysics(false);
		GetSkeletalMeshComponent()->SetVisibility(false);

		// Broadcast HitDamage (die in explostion) or SoftDamage (shock) to soldiers within the sphere
		for (int i = 0; i < AffectedSoldiers.Num(); i++) {
			FVector MyLoc = GetActorLocation();
			FVector vDir = AffectedSoldiers[i]->GetActorLocation() - MyLoc;
			float dist = vDir.Size();
			if (dist > BOMB_MAX_DAMAGE_RADIUS) dist = BOMB_MAX_DAMAGE_RADIUS;

			vDir.Normalize();
			Damage = MaxDamage * (1 - dist / BOMB_MAX_DAMAGE_RADIUS);

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "BombHit to: " + AffectedSoldiers[i]->GetName() + " with Damage = " + FString::SanitizeFloat(Damage));

			if (Damage >= 1.0f) {
				AffectedSoldiers[i]->OnHitDamage(1.0f, MyLoc, vDir);	// good night sweet prince
			}
			else {
				AffectedSoldiers[i]->OnSoftDamage(Damage, MyLoc, vDir);	// stand up and sing, but later
			}
		}

		SetLifeSpan(3.0f);
	}
	else {
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void ABomb::External_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsPassive || OtherComp->ComponentHasTag(TEXT("HeadCollider"))) return;

	if (GetSkeletalMeshComponent()->IsSimulatingPhysics() && CollisionComp->GetCollisionEnabled() == ECollisionEnabled::Type::QueryOnly) {
		if (!OtherActor->IsA(AHandController::StaticClass()))
			OnHit(OverlappedComponent, OtherActor, OtherComp, FVector::ZeroVector, SweepResult);
	}
	else if (!bIsAttached && OtherActor->IsA(AHandController::StaticClass())) {
		AHandController *ctrl = Cast<AHandController>(OtherActor);
		if (ctrl != nullptr && ctrl->ControllerMode == EControllerTool::C_Tool && !ctrl->IsHoldingStaticMesh()) {
			bIsAttached = true;
			GetSkeletalMeshComponent()->SetSimulatePhysics(false);
			GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//this->AttachRootComponentToActor(ctrl);

			this->SetActorLocation(ctrl->ToolMesh->GetSocketLocation(TEXT("Socket_DataDevice")));
			this->SetActorRotation(ctrl->ToolMesh->GetSocketQuaternion(TEXT("Socket_DataDevice")));
			this->AttachToComponent(ctrl->ToolMesh, FAttachmentTransformRules::KeepWorldTransform, TEXT("Socket_DataDevice"));

			if (ctrl->TakeStaticMesh(GetSkeletalMeshComponent(), nullptr)) {
				ctrl->CurrentBomb = this;
				ctrl->SetToolTrace(false);
			}
			AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			player->PlayVoice(EVoiceQuote::VO_C_Gl_Grenades);

			OnPickUp();
		}
	}
}

void ABomb::SetState(bool NewState)
{
	if (bState != NewState) {
		bState = NewState;

		if (bState) {
			if (IsValid(mti_Light)) mti_Light->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor::Yellow);
			GetSkeletalMeshComponent()->PlayAnimation(AnimOpen, false);
		} else {
			if (IsValid(mti_Light)) mti_Light->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor::Green);
			GetSkeletalMeshComponent()->PlayAnimation(AnimClose, false);
		}
	}
}

void ABomb::Throw(FVector LinearSpeed)
{
	DetachRootComponentFromParent(true);
	//AttachRootComponentToActor(this);
	//CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	GetSkeletalMeshComponent()->SetSimulatePhysics(true);
	GetSkeletalMeshComponent()->AddImpulse(LinearSpeed * 5.0f);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	/*
	if (!hActivateCollisionTimer.IsValid()) {
		GetWorldTimerManager().SetTimer(hActivateCollisionTimer, this, &ABomb::OnTimer_ActivateCollision, 0.25f, false);
	}
	*/

	if (bState) {
		SetLifeSpan(7.0f);
		ExternalCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	bIsAttached = false;
}

void ABomb::OnTimer_ActivateCollision()
{
	GetWorldTimerManager().ClearTimer(hActivateCollisionTimer);
	hActivateCollisionTimer.Invalidate();

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	CollisionComp->SetNotifyRigidBodyCollision(true);
}

void ABomb::Trigger_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ASoldierBasic::StaticClass())) {
		//bool 
		AffectedSoldiers.AddUnique(Cast<ASoldierBasic>(OtherActor));
	}
}

void ABomb::Trigger_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (DisableSoldierArray) return;
	if (OtherActor->IsA(ASoldierBasic::StaticClass())) {
		for (int i = 0; i < AffectedSoldiers.Num(); i++) {
			if (AffectedSoldiers[i]->GetUniqueID() == OtherActor->GetUniqueID()) {
				AffectedSoldiers.RemoveAt(i);
				break;
			}
		}
	}
}