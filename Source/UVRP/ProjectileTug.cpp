// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "GamePlatform.h"
#include "GunSlingComponent.h"
#include "HandController.h"
#include "UVRPGameMode.h"
#include "UVRPCharacter.h"
#include "ProjectileTug.h"

AProjectileTug::AProjectileTug()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	if (!IsValid(RootComponent)) RootComponent = CollisionComp;
		
	BombMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(CollisionComp);
	//RootComponent = BombMesh;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smAsset(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/GunSling/sm_ProjectileHitch.sm_ProjectileHitch'"));
	if (smAsset.Object != NULL) BombMesh->SetSkeletalMesh(smAsset.Object);
	BombMesh->SetMobility(EComponentMobility::Movable);
	BombMesh->SetRelativeLocation(FVector::ZeroVector);

	CollisionComp->SetSimulatePhysics(false);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> smAnimAsset(TEXT("AnimSequence'/Game/FirstPerson/Objects/GunSling/anim_ProjectileHitch.anim_ProjectileHitch'"));
	if (smAnimAsset.Object != NULL) AnimOpen = smAnimAsset.Object;

	BeamLightning = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BeamLightning"));
	BeamLightning->SetupAttachment(CollisionComp);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psExAsset(TEXT("ParticleSystem'/Game/FirstPerson/Effects/ps_LightningBeam.ps_LightningBeam'"));
	if (psExAsset.Object != NULL) BeamLightning->SetTemplate(psExAsset.Object);
	BeamLightning->bAutoActivate = false;
	bCanBePulled = false;

	fLifeSpanAfterFire = 10.0f;
	TimeInFlight = 0.0f;
}

void AProjectileTug::BeginPlay()
{
	Super::BeginPlay();

	//BeamLightning->SetBeamSourcePoint(0, GetActorLocation(), 1);
	//BeamLightning->AttachTo(GetWorld()->GetFirstPlayerController()->GetPawn()->GetRootComponent());
	//BeamLightning->SetWorldLocation(FVector::ZeroVector);
	//BeamLightning->SetActorParameter(TEXT("BeamSource"), GetWorld()->GetFirstPlayerController()->GetPawn());

	//BeamLightning->SetVectorParameter(TEXT("BeamSource"), FVector::ZeroVector);
	//BeamLightning->SetActorParameter(TEXT("BeamSource"), GetWorld()->GetFirstPlayerController()->GetPawn());

	//BeamLightning->SetVectorParameter(TEXT("BeamTarget"), FVector(800.0f, 0.0f, 150.0f));
	//BeamLightning->SetActorParameter(TEXT("BeamSource"), this->GetWorld()->GetFirstPlayerController()->GetPawn());
	BeamLightning->SetActorParameter(TEXT("BeamTarget"), this);
	//BeamLightning->SetBeamTargetPoint(0, GetActorLocation(), 1);
	//BeamLightning->ResetParticles();
	//BeamLightning->SetActorParameter("BeamSource", this);
	//BeamLightning->Deactivate();
}


void AProjectileTug::InitializeProjectileFeedback(AUVRPCharacter* Player, bool IsRightHand, FHandFeedbackSetup OptFeedbackWave)
{
	AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerOwner = Player;
	IsRightController = IsRightHand;

	if (IsValid(gm)) {
		FeedbackWave = OptFeedbackWave; //gm->GetControllerFeedbackOptions(EFeedbackAction::A_SlingPull);
	}
}


void AProjectileTug::OnDestroyProjectile()
{
	/////////////////// FEEDBACK ///////////////////
	if (IsValid(PlayerOwner)) {
		PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Stop);
	}
	////////////////////////////////////////////////
	if (IsValid(ParentGun)) ParentGun->ReleaseProjectile();
}

void AProjectileTug::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentWavePos += DeltaSeconds;
	if (CurrentWavePos >= WaveLength && FMath::Abs(CurrentWaveValue) <= 0.05f) {
		CurrentWaveValue = 0.0f;
		SetActorTickEnabled(false);
		bCanBePulled = true;

		/////////////////// FEEDBACK ///////////////////
		if (IsValid(PlayerOwner)) {
			PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Stop);
		}
		////////////////////////////////////////////////
	}
	else {
		CurrentWaveValue = -FMath::Cos(CurrentWavePos * 4.0f) * FMath::Clamp(1 - CurrentWavePos / WaveLength, 0.0f, 1.0f);
	}

	BeamLightning->SetVectorParameter(TEXT("Tangent"), FVector(0.0f, 0.0f, CurrentWaveValue / 2.0f + 0.5f));
}

void AProjectileTug::ActivateProjectile(USceneComponent* AttachParent, FVector AngularImpulse, FVector LinearImpulse)
{
	Super::ActivateProjectile(AttachParent, AngularImpulse, LinearImpulse);

	SetActorTransform(AttachParent->GetSocketTransform("FireSocket"));
	DetachRootComponentFromParent(true);
	BombMesh->PlayAnimation(AnimOpen, false);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->ProjectileMovement->SetVelocityInLocalSpace(FVector(LinearImpulse.Size(), 0.0f, 0.0f));
	//ProjectileMovement->Velocity = GetActorRotation().Vector() * LinearImpulse.Size();
	this->ProjectileMovement->Velocity = LinearImpulse;
	this->ProjectileMovement->InitialSpeed = LinearImpulse.Size();
	this->ProjectileMovement->ProjectileGravityScale = 1.0f;
	this->ProjectileMovement->SetActive(true);
	this->ProjectileMovement->Activate();

	BeamLightning->SetVectorParameter(TEXT("Tangent"), FVector(0.0f, 0.0f, 0.0f));

	BeamLightning->Activate();
	BeamLightning->SetActive(true);
	SetLifeSpan(fLifeSpanAfterFire);

	/////////////////// FEEDBACK ///////////////////
	/*
	if (IsValid(PlayerOwner)) {
		PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Start);
	}
	*/
	////////////////////////////////////////////////

	GetWorldTimerManager().SetTimer(hCheckBeamTimer, this, &AProjectileTug::TimerCheckBeam, 0.1f, true);
}

void AProjectileTug::OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "AProjectileTug::OnHit");

	//if (OtherActor->IsA(APawn::StaticClass())) 

	if (OtherActor->IsA(AHandController::StaticClass())) return;
	if (hCheckBeamTimer.IsValid()) GetWorldTimerManager().ClearTimer(hCheckBeamTimer);

	//StartWavingTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	CurrentWavePos = 0.0f;
	CurrentWaveValue = 0.0f;
	WaveLength = 3.0f;
	
	SetActorTickEnabled(true);
	if (OtherComp->ComponentHasTag("Shield")) {
		AttachedShield = OtherComp;

		if (OtherActor->IsA(AGamePlatform::StaticClass()))
			AttachedPlatform = Cast<AGamePlatform>(OtherActor);
	}
	else {
		TimerKillBeam();
		OnDestroyProjectile();
		SetLifeSpan(2.5f);
	}

	/////////////////// FEEDBACK ///////////////////
	if (IsValid(PlayerOwner)) {
		PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Stop);
	}
	////////////////////////////////////////////////


	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, OtherComp->GetName());
}

void AProjectileTug::SetVisibility(bool NewVisibility)
{
	Super::SetVisibility(NewVisibility);

	BombMesh->SetVisibility(NewVisibility);
}

void AProjectileTug::InitializeProjectile(USceneComponent* ParentGun)
{
	BeamLightning->AttachToComponent(ParentGun, FAttachmentTransformRules::KeepRelativeTransform, TEXT("FireSocket"));
	BeamLightning->SetRelativeLocation(FVector::ZeroVector);
	BeamLightning->SetRelativeRotation(FRotator::ZeroRotator); // ParentGun->GetSocketRotation(TEXT("FireSocket")));
}

bool AProjectileTug::IsDetached()
{
	return bCanBePulled;
}

bool AProjectileTug::ForcePull(FVector Direction, float Force)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "pull!");
	bool bRet = false;
	/////////////////// FEEDBACK ///////////////////
	if (IsValid(PlayerOwner)) {
		PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Stop);
	}
	////////////////////////////////////////////////

	if (IsValid(AttachedShield)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, AttachedShield->GetName());
		this->AttachToComponent(AttachedShield, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		AttachedShield->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		AttachedShield->SetSimulatePhysics(true);
		//AttachedShield->WakeRigidBody();
		AttachedShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		AttachedShield->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		AttachedShield->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttachedShield->AddImpulseAtLocation(Direction * Force * 10.0f, GetActorLocation());
		AttachedShield->AddRelativeLocation(FVector(0.0f, 0.0f, 2.0f));
		//SetLifeSpan(2.0f);
		//BeamLightning->SetActive(false);
		//BeamLightning->Deactivate();
		FTimerHandle h;
		GetWorldTimerManager().SetTimer(h, this, &AProjectileTug::TimerKillBeam, 0.5f, false);

		if (IsValid(AttachedPlatform)) {
			if (AttachedShield->ComponentHasTag("1"))
				AttachedPlatform->ShieldState1 = false;
			else if (AttachedShield->ComponentHasTag("2"))
				AttachedPlatform->ShieldState2 = false;
			else if (AttachedShield->ComponentHasTag("3"))
				AttachedPlatform->ShieldState3 = false;
			else if (AttachedShield->ComponentHasTag("4"))
				AttachedPlatform->ShieldState4 = false;
		}
		AttachedShield = nullptr;
		AttachedPlatform = nullptr;

		bRet = true;
	}

	SetLifeSpan(1.0f);

	return bRet;
}

void AProjectileTug::TimerKillBeam()
{
	BeamLightning->SetActive(false);
	BeamLightning->Deactivate();
	BeamLightning->DeactivateSystem();
	BeamLightning->SetVisibility(false);
}

void AProjectileTug::TimerCheckBeam()
{
	FVector vSrcPoint = BeamLightning->GetComponentLocation();
	FVector vTargetPoint = GetActorLocation();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.AddIgnoredActor(this);
	RV_TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	RV_TraceParams.AddIgnoredComponent(BeamLightning);
	FHitResult RV_Hit(ForceInit);

	TimeInFlight += 0.1f;
	/////////////////// FEEDBACK ///////////////////
	if (IsValid(PlayerOwner)) {
		float intensity = FMath::Clamp(TimeInFlight / 2.0f, 0.0f, 1.0f);
		//if ((FeedbackWave.DefaultIntensity - intensity) >= 0.05f) {
			FeedbackWave.DefaultIntensity = intensity;
			FeedbackWave.DefaultDuration = 0.1f;
			PlayerOwner->UpdateFeedbackByAction(FeedbackWave, IsRightController, EDynamicForceFeedbackAction::Start);
		//}
	}
	////////////////////////////////////////////////

	/*
	GetWorld()->LineTraceSingleByChannel(RV_Hit, vSrcPoint, vTargetPoint, ECollisionChannel::ECC_EngineTraceChannel1, RV_TraceParams);
	if (RV_Hit.bBlockingHit && RV_Hit.GetActor() != nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, RV_Hit.GetActor()->GetName());
		TimerKillBeam();
		SetLifeSpan(0.5f);
		GetWorldTimerManager().ClearTimer(hCheckBeamTimer);
	}*/
}