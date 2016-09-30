// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "TeleportRingsSelector.h"
#include "UGunComponent.h"
#include "ControllerMenuPointer.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterVoicePreset.h"
#include "UVRPCharacter.h"
#include "DataLock.h"
#include "DataJigsaw.h"
#include "Teleport.h"
#include "MenuCollisionBox.h"
#include "GunSlingComponent.h"
#include "Bomb.h"
#include "HandMenuSlider.h"
#include "UVRPProjectile.h"

#include "HandController.h"

// Sets default values
AHandController::AHandController()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smGunSmall(TEXT("StaticMesh'/Game/FirstPerson/Meshes/gun.gun'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smGunLarge(TEXT("StaticMesh'/Game/FirstPerson/Meshes/gun2_new.gun2_new'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smToolMesh(TEXT("StaticMesh'/Game/FirstPerson/Meshes/tool_basic.tool_basic'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smToolMenu(TEXT("StaticMesh'/Game/FirstPerson/Meshes/toolmenudisplay.toolmenudisplay'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smShield(TEXT("SkeletalMesh'/Game/FirstPerson/Meshes/sm_PlayerShield.sm_PlayerShield'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> txControllerMenu1(TEXT("Texture2D'/Game/FirstPerson/Textures/displays/controller_menu_1.controller_menu_1'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> txControllerMenu2(TEXT("Texture2D'/Game/FirstPerson/Textures/displays/controller_menu_2.controller_menu_2'"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smLeftHand(TEXT("StaticMesh'/Game/FirstPerson/Objects/Hands/character_handleft.character_handleft'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smRightHand(TEXT("StaticMesh'/Game/FirstPerson/Objects/Hands/character_handright.character_handright'"));
	if (smLeftHand.Object != NULL) LeftHandStaticMesh = smLeftHand.Object;
	if (smRightHand.Object != NULL) RightHandStaticMesh = smRightHand.Object;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Инициализация компонентов
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MainRootComponent"));
	SetRootComponent(SceneRootComponent);
	SceneRootComponent->SetMobility(EComponentMobility::Type::Movable);

	/*
	MainRootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MainRootArrow"));
	MainRootArrow->AttachParent = GetRootComponent();
	MainRootArrow->SetMobility(EComponentMobility::Type::Movable);
	MainRootArrow->SetRelativeLocation(FVector(-6.5f, 0.0f, -7.0f));
	MainRootArrow->SetRelativeRotation(FRotator(-65.0f, 0.0f, 0.0f));
	MainRootArrow->ArrowSize = 0.25f;
	//MainRootArrow->RegisterComponent();
	*/

	HandStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandStaticMesh"));
	HandStaticMesh->SetupAttachment(GetRootComponent());
	HandStaticMesh->SetRelativeLocation(FVector(-8.789f, 2.517f, 1.855f));
	HandStaticMesh->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));
	HandStaticMesh->SetStaticMesh(RightHandStaticMesh);
	HandStaticMesh->SetCastShadow(false);

	GunSmall = CreateDefaultSubobject<UUGunComponent>(TEXT("GunSmall"));
	GunSmall->SetupAttachment(HandStaticMesh, TEXT("Socket_GunSmall"));
	GunSmall->SetMobility(EComponentMobility::Type::Movable);
	GunSmall->SetRelativeLocation(FVector::ZeroVector);
	GunSmall->SetRelativeRotation(FRotator::ZeroRotator);
	GunSmall->AutoFiring = false;
	GunSmall->UsedByPlayer = true;
	GunSmall->UpdateMaterialEmission = true;
	GunSmall->SetCastShadow(false);
	if (smGunSmall.Object != NULL) 
		GunSmall->SetStaticMesh((UStaticMesh*)smGunSmall.Object);
	//GunSmall->SetRelativeLocation(MainRootArrow->GetRelativeTransform().GetLocation());
	//GunSmall->SetRelativeRotation(MainRootArrow->GetRelativeTransform().GetRotation());
	//GunSmall->RegisterComponent();

	ToolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMesh"));
	ToolMesh->SetupAttachment(HandStaticMesh, TEXT("Socket_Tool"));
	ToolMesh->SetMobility(EComponentMobility::Type::Movable);
	ToolMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ToolMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ToolMesh->SetCastShadow(false);
	if (smToolMesh.Object != NULL)
		ToolMesh->SetStaticMesh((UStaticMesh*)smToolMesh.Object);
	//ToolMesh->RegisterComponent();

	ToolBase = CreateDefaultSubobject<USceneComponent>(TEXT("ToolBase"));
	ToolBase->SetupAttachment(ToolMesh, TEXT("RaySource"));
	ToolBase->SetMobility(EComponentMobility::Type::Movable);
	ToolBase->SetRelativeLocation(FVector(-1.133f, 0.0f, 0.2f));
	ToolBase->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	//ToolBase->RegisterComponent();

	ToolPointer = CreateDefaultSubobject<UControllerMenuPointer>(TEXT("ToolPointer"));
	ToolPointer->SetupAttachment(ToolMesh, TEXT("RaySource"));
	ToolPointer->SetMobility(EComponentMobility::Type::Movable);
	ToolPointer->SetRelativeLocation(FVector(-1.133f, 0.0f, 0.2f));
	ToolPointer->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	//ToolPointer->RegisterComponent();

	ToolCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ToolCollider"));
	ToolCollider->SetupAttachment(ToolBase);
	ToolCollider->SetMobility(EComponentMobility::Type::Movable);
	ToolCollider->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
	ToolCollider->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ToolCollider->SetBoxExtent(FVector(50.0f, 0.1f, 0.1f));
	ToolCollider->ComponentTags.Add(TEXT("ControllerToolLine"));
	ToolCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ToolCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ToolCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ToolCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Overlap);
	ToolCollider->bGenerateOverlapEvents = true;
	ToolCollider->OnComponentBeginOverlap.AddDynamic(this, &AHandController::OnBeginOverlap_MenuCollider);
	ToolCollider->OnComponentEndOverlap.AddDynamic(this, &AHandController::OnEndOverlap_MenuCollider);
	ToolCollider->bHiddenInGame = true;
	//ToolCollider->RegisterComponent();

	GunLarge = CreateDefaultSubobject<UUGunComponent>(TEXT("GunLarge"));
	GunLarge->SetupAttachment(HandStaticMesh, TEXT("Socket_GunLarge"));
	GunLarge->SetMobility(EComponentMobility::Type::Movable);
	GunLarge->SetRelativeLocation(FVector::ZeroVector);
	GunLarge->SetRelativeRotation(FRotator::ZeroRotator);
	GunLarge->AutoFiring = false;
	GunLarge->UsedByPlayer = true;
	GunLarge->ShootingSpeedInterval = 0.2f;
	GunLarge->ShotTemperatureIncrement = 0.05f;
	GunLarge->SetCastShadow(false);
	if (smGunLarge.Object != NULL)
		GunLarge->SetStaticMesh((UStaticMesh*)smGunLarge.Object);
	//GunLarge->RegisterComponent();

	HitMenuCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("HitMenuCollider"));
	HitMenuCollider->SetupAttachment(ToolPointer, "HitSocket");
	HitMenuCollider->SetMobility(EComponentMobility::Type::Movable);
	HitMenuCollider->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HitMenuCollider->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	HitMenuCollider->SetBoxExtent(FVector(1.0f, 1.0f, 1.0f));
	HitMenuCollider->ComponentTags.Add("HandMenuPointer");
	HitMenuCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	HitMenuCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HitMenuCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitMenuCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	HitMenuCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Overlap);
	HitMenuCollider->bGenerateOverlapEvents = true;
	//HitMenuCollider->RegisterComponent();

	PositionalTrigger1 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger1"));
	PositionalTrigger1->SetupAttachment(ToolMesh, "TriggersSocket");
	PositionalTrigger1->SetMobility(EComponentMobility::Type::Movable);
	PositionalTrigger1->SetRelativeLocation(FVector(-4.13f, 0.0f, 0.8f));
	PositionalTrigger1->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	PositionalTrigger1->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger1->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//PositionalTrigger1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger1->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger1->OnComponentBeginOverlap.AddDynamic(this, &AHandController::Trigger1_OnBeginOverlap);
	PositionalTrigger1->OnComponentEndOverlap.AddDynamic(this, &AHandController::Trigger1_OnEndOverlap);
	PositionalTrigger1->bHiddenInGame = true;
	//PositionalTrigger1->ComponentTags.Add(TAG_DataLock);
	//PositionalTrigger1->RegisterComponent();
	
	PositionalTrigger2 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger2"));
	PositionalTrigger2->SetupAttachment(ToolMesh, "TriggersSocket");
	PositionalTrigger2->SetMobility(EComponentMobility::Type::Movable);
	PositionalTrigger2->SetRelativeLocation(FVector(2.67f, 3.2f, 0.8f));
	PositionalTrigger2->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	PositionalTrigger2->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger2->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//PositionalTrigger2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger2->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger2->OnComponentBeginOverlap.AddDynamic(this, &AHandController::Trigger2_OnBeginOverlap);
	PositionalTrigger2->OnComponentEndOverlap.AddDynamic(this, &AHandController::Trigger2_OnEndOverlap);
	PositionalTrigger2->bHiddenInGame = true;
	//PositionalTrigger2->ComponentTags.Add(TAG_DataLock);
	//PositionalTrigger2->RegisterComponent();

	PositionalTrigger3 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger3"));
	PositionalTrigger3->SetupAttachment(ToolMesh, "TriggersSocket");
	PositionalTrigger3->SetMobility(EComponentMobility::Type::Movable);
	PositionalTrigger3->SetRelativeLocation(FVector(2.67f, -3.2f, 0.8f));
	PositionalTrigger3->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	PositionalTrigger3->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger3->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//PositionalTrigger3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger3->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger3->OnComponentBeginOverlap.AddDynamic(this, &AHandController::Trigger3_OnBeginOverlap);
	PositionalTrigger3->OnComponentEndOverlap.AddDynamic(this, &AHandController::Trigger3_OnEndOverlap);
	PositionalTrigger3->bHiddenInGame = true;
	//PositionalTrigger3->ComponentTags.Add(TAG_DataLock);
	//PositionalTrigger3->RegisterComponent();

	GunSuper = CreateDefaultSubobject<UGunSlingComponent>(TEXT("GunSuper"));
	GunSuper->SetupAttachment(HandStaticMesh, TEXT("Socket_GunSling"));
	GunSuper->SetMobility(EComponentMobility::Type::Movable);
	GunSuper->SetRelativeLocation(FVector::ZeroVector);
	GunSuper->SetRelativeRotation(FRotator::ZeroRotator);
	//GunSuper->RegisterComponent();

	ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(HandStaticMesh, TEXT("Socket_Shield"));
	ShieldMesh->SetMobility(EComponentMobility::Type::Movable);
	ShieldMesh->SetRelativeLocation(FVector::ZeroVector);
	ShieldMesh->SetRelativeRotation(FRotator::ZeroRotator);
	if (smShield.Object != NULL)
		ShieldMesh->SetSkeletalMesh(smShield.Object);
	ShieldMesh->SetVisibility(false);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	ShieldMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	/* Activate this string to make Two-pistols Soldiers see shield */
	ShieldMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	/****************************************************************/
	ShieldMesh->SetCollisionResponseToChannel(EEC_ProjectileChannel, ECollisionResponse::ECR_Block);
	ShieldMesh->SetCastShadow(false);
	ShieldMesh->SetNotifyRigidBodyCollision(true);
	ShieldMesh->OnComponentHit.AddDynamic(this, &AHandController::OnShieldHit);
	ShieldMesh->ComponentTags.Add(TEXT("Shield"));

	ToolMenuMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMenuMesh"));
	ToolMenuMesh->SetupAttachment(ToolMesh, TEXT("ToolMenuSocket"));
	ToolMenuMesh->SetRelativeLocation(FVector::ZeroVector);
	ToolMenuMesh->SetRelativeRotation(FRotator::ZeroRotator);
	ToolMenuMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ToolMenuMesh->SetVisibility(false);
	ToolMenuMesh->SetCastShadow(false);
	if (smToolMenu.Object != NULL)
		ToolMenuMesh->SetStaticMesh(smToolMenu.Object);


	static ConstructorHelpers::FObjectFinder<USoundBase> swClickSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/273_vjik.273_vjik'"));
	if (swClickSound.Object != NULL) OnClickSound = (USoundBase*)swClickSound.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> swThrowSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/504_vjik.504_vjik'"));
	if (swThrowSound.Object != NULL) OnThrowSound = (USoundBase*)swThrowSound.Object;		

	static ConstructorHelpers::FObjectFinder<UAnimSequence> ansqShieldOpen(TEXT("AnimSequence'/Game/FirstPerson/Meshes/anim_PlayerShield_Open.anim_PlayerShield_Open'"));
	if (ansqShieldOpen.Object != NULL) anim_ShieldOpen = (UAnimSequence*)ansqShieldOpen.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> ansqShieldClose(TEXT("AnimSequence'/Game/FirstPerson/Meshes/anim_PlayerShield_Close.anim_PlayerShield_Close'"));
	if (ansqShieldClose.Object != NULL) anim_ShieldClose = (UAnimSequence*)ansqShieldClose.Object;

	if (txControllerMenu1.Object != NULL)
		tx_ToolMenuDisplay1 = txControllerMenu1.Object;
	if (txControllerMenu2.Object != NULL)
		tx_ToolMenuDisplay2 = txControllerMenu2.Object;

	TriggersState[0] = TriggersState[1] = TriggersState[2] = false;
	HoldingMesh1 = nullptr;
	HoldingMesh2 = nullptr;
	HandSide = EHandSide::S_Right;
	CurrentDataDevice = -1;
	CurrentToolMenuDisplay = false;

	IsToolActive = true;
	IsGunSmallActive = true;
	IsGunLargeActive = false;
	IsGunSuperActive = false;
	IsShieldActive = false;
	IsShieldAnimationFinished = 0;

	UpdateControllerMode(EControllerTool::C_Tool);

	if (IsValid(ToolMesh)) {
		mti_ToolMenu = ToolMenuMesh->CreateDynamicMaterialInstance(0);
	}
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();

	//UpdateControllerMode(EControllerTool::C_Tool);
	//InitialOffset = MainRootArrow->GetRelativeTransform().GetLocation();
	FireSocketSpeed = FVector::ZeroVector;
	PrevTimerTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	GetWorldTimerManager().SetTimer(hMainTimer, this, &AHandController::TimerUpdateFunction, 0.02f, true, 0.0f);

	PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (IsValid(ShieldMesh))
		mti_ShieldField = ShieldMesh->CreateDynamicMaterialInstance(1);

	if (!IsValid(mti_ToolMenu) && IsValid(ToolMenuMesh)) {
		mti_ToolMenu = ToolMenuMesh->CreateDynamicMaterialInstance(0);
	}
	SetToolMenuVisibility(false);

	HitMenuCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HitMenuCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	HitMenuCollider->bHiddenInGame = true;
}

void AHandController::TimerUpdateFunction()
{
	//float CurrentTime, DeltaTime;
	//CurrentTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	//DeltaTime = CurrentTime - PrevTimerTime;
	const float DeltaTime = 0.02f;

	// 1. Обновить длину луча для Tool
	if (ControllerMode == EControllerTool::C_Tool && ToolPointer->IsVisible()) {
		ToolPointer->TracePlayerSelectorLine(DeltaTime);
	}

	// 2. Считаем скорость движения дула
	FVector NewLoc = FVector::ZeroVector;
	if (ControllerMode == EControllerTool::C_Gun_Small)
		NewLoc = GunSmall->GetSocketLocation("FireSource");
	else if (ControllerMode == EControllerTool::C_Gun_Large)
		NewLoc = GunLarge->GetSocketLocation("FireSource");
	else if (ControllerMode == EControllerTool::C_Gun_Sling)
		NewLoc = GunSuper->GetSocketLocation("ProjectileBaseSocket" /*"FireSocket"*/);


	FVector lin = PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right) * 5.0f;

	if (NewLoc != FVector::ZeroVector && ControllerMode == EControllerTool::C_Gun_Sling) {
		if (DeltaTime != 0.0f) {
			FVector NewSpeed = (NewLoc - PreviousSocketLocation) / DeltaTime;

			FireSocketSpeed = (FireSocketSpeed + NewSpeed) / 2.0f;
			
			float forcesize = FireSocketSpeed.Size();
			if (forcesize > PULL_MinForce && GunSuper->HasTug() && PreviousSocketLocation != FVector::ZeroVector) {
				FVector p1 = NewLoc - GunSuper->GetTugLocation(); p1.Normalize();
				FVector p2 = NewSpeed; p2.Normalize();

				// pull (not push) force - by vector multiply
				// cos a = ab / (|a| |b|)
				// cos > 0 = pull, cos < 0 = push
				if (FVector::DotProduct(p1, p2) / (p1.Size() * p2.Size()) > 0.0f) 
					GunSuper->TranslatePullForce(p1, forcesize);
			}

			PreviousSocketLocation = NewLoc;
			//PrevTimerTime = CurrentTime;
		}
	}
	else if (PreviousSocketLocation != FVector::ZeroVector) {
		PreviousSocketLocation = FVector::ZeroVector;
	}
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	//TimerUpdateFunction();
	//DrawDebugBox(GetWorld(), ToolCollider->GetComponentLocation(), ToolCollider->GetScaledBoxExtent(), ToolCollider->GetComponentQuat(), FColor::Red);
}

void AHandController::SetHandSide(EHandSide NewSide)
{
	if (HandSide != NewSide) {
		HandSide = NewSide;

		if (HandSide == EHandSide::S_Right) {
			HandStaticMesh->SetStaticMesh(RightHandStaticMesh);
			HandStaticMesh->SetRelativeLocation(FVector(-8.789f, 2.517f, 1.855f));
			HandStaticMesh->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));
		}
		else {
			HandStaticMesh->SetStaticMesh(LeftHandStaticMesh);
			HandStaticMesh->SetRelativeLocation(FVector(-8.789f, -2.517f, 1.855f));
			HandStaticMesh->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));
		}

		ToolMesh->AttachToComponent(HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Socket_Tool"));
		GunSmall->AttachToComponent(HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Socket_GunSmall"));
		GunLarge->AttachToComponent(HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Socket_GunLarge"));
		GunSuper->AttachToComponent(HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Socket_GunSling"));
		ShieldMesh->AttachToComponent(HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Socket_Shield"));

		ToolMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator.Quaternion());
		GunSmall->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator.Quaternion());
		GunLarge->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator.Quaternion());
		GunSuper->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator.Quaternion());
		ShieldMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator.Quaternion());
	}
}

void AHandController::ActivateHandComponent(EControllerTool Component)
{
	if (Component == EControllerTool::C_Tool) {
		IsToolActive = true;
	}
	else if (Component == EControllerTool::C_Gun_Small) {
		IsGunSmallActive = true;
		PlayerOwner->SetGunSmallActive(true);
	}
	else if (Component == EControllerTool::C_Gun_Large) {
		IsGunLargeActive = true;
		PlayerOwner->SetGunLargeActive(true);
	}
	else if (Component == EControllerTool::C_Gun_Sling) {
		IsGunSuperActive = true;
		PlayerOwner->SetGunSlingActive(true);
	}
	else if (Component == EControllerTool::C_Shield) {
		IsShieldActive = true;
		PlayerOwner->SetShieldActive(true);
	}
}

FVector AHandController::GetToolPointerBlockingPoint()
{
	return ToolPointer->TraceBlockingPoint;
}

void AHandController::SetToolMenuVisibility(bool IsVisible)
{
	if (IsVisible) {
		SetToolMenuDisplay(false);
	}
	ToolMenuMesh->SetVisibility(IsVisible);

	if (IsValid(PlayerOwner)) PlayerOwner->SetHandMeshVisibility(HandSide == EHandSide::S_Right, !IsVisible);
}

void AHandController::SetToolMenuActiveSlot(FVector2D State)
{
	ToolMenuActiveSlot = 0;
	uint8 NewSlot = 0;
	if (State.Size() > 0.2f) {
		float angle = 45.0f + FMath::RadiansToDegrees(FMath::Atan2(State.Y, State.X));
		if (angle < 0.0f) angle += 360.0f;
		if (angle >= 360.0f) angle -= 360.0f;

		if (angle <= 30)
			NewSlot = 1;
		else if (angle <= 60)
			NewSlot = 2;
		else if (angle <= 120)
			NewSlot = 3;
		else if (angle <= 180)
			NewSlot = 4;
		else if (angle <= 240)
			NewSlot = 5;
		else if (angle <= 360)
			NewSlot = 6;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "current slot is " + FString::FromInt(NewSlot) + " and angle is " + FString::SanitizeFloat(angle));
	}

	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_Gun_Small && !IsGunSmallActive) NewSlot = 0;
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_Gun_Large && !IsGunLargeActive) NewSlot = 0;
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_Gun_Sling && !IsGunSuperActive) NewSlot = 0;
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_Shield && !IsShieldActive) NewSlot = 0;
	
	BYTE datamodules;
	for (int i = 0; i < PlayerOwner->ControllerDataDevices.Num(); i++) {
		if (PlayerOwner->ControllerDataDevices[i]->UniqueNumber == 2) datamodules |= 1;
		else if (PlayerOwner->ControllerDataDevices[i]->UniqueNumber == 3) datamodules |= 2;
		else if (PlayerOwner->ControllerDataDevices[i]->UniqueNumber == 4) datamodules |= 4;
	}
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_ModuleBlue && !(datamodules   || 1)) NewSlot = 0;
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_ModuleGreen && !(datamodules  || 2)) NewSlot = 0;
	if (GetCurrentToolMenuSlot(NewSlot) == EControllerTool::C_ModuleViolet && !(datamodules || 4)) NewSlot = 0;
	/*
	001
	010
	011
	100
	*/
	ToolMenuActiveSlot = NewSlot;

	if (IsValid(mti_ToolMenu))
		mti_ToolMenu->SetScalarParameterValue(TEXT("Slot"), (float)ToolMenuActiveSlot);
}

void AHandController::SetToolMenuDisplay(bool ShowDataModulesMenu)
{
	if (!IsValid(mti_ToolMenu)) return;

	if (CurrentToolMenuDisplay != ShowDataModulesMenu) {
		if (ShowDataModulesMenu)
			mti_ToolMenu->SetTextureParameterValue(TEXT("SourceDisplay"), tx_ToolMenuDisplay2);
		else
			mti_ToolMenu->SetTextureParameterValue(TEXT("SourceDisplay"), tx_ToolMenuDisplay1);

		CurrentToolMenuDisplay = ShowDataModulesMenu;
		SetToolMenuActiveSlot(FVector2D(0.0f, 0.0f));
	}
}

EControllerTool AHandController::GetCurrentToolMenuSlot(uint8 CustomSlot)
{
	if (CustomSlot == 255) CustomSlot = ToolMenuActiveSlot;

	EControllerTool eRet;
	switch (CustomSlot) {
		case 1:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Tool : EControllerTool::C_Tool;
			break;
		case 2:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Gun_Small : EControllerTool::C_ModuleRed;
			break;
		case 3:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Gun_Large : EControllerTool::C_ModuleBlue;
			break;
		case 4:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Gun_Sling : EControllerTool::C_ModuleGreen;
			break;
		case 5:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Shield : EControllerTool::C_ModuleViolet;
			break;
		case 6:
			eRet = !CurrentToolMenuDisplay ? EControllerTool::C_Additional : EControllerTool::C_Tool;
			break;
		default:
			eRet = EControllerTool::C_None; // == null
			break;
	}
	return eRet;
}

void AHandController::SetShieldColor(FLinearColor Color)
{
	if (!IsValid(mti_Shield)) {
		mti_Shield = ShieldMesh->CreateDynamicMaterialInstance(1);
	}
	if (IsValid(mti_Shield)) {
		mti_Shield->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AHandController::OnTriggerStateChanged(bool IsPressedOrReleased)
{
	ControllerTriggerState = IsPressedOrReleased;

	if (IsValid(CurrentJigsaw)) {
		if (IsPressedOrReleased)
			CurrentJigsaw->OnControllerTriggerPressed(this);
		else
			CurrentJigsaw->OnControllerTriggerReleased(this);
	}
}

void AHandController::UpdateControllerMode(EControllerTool NewMode)
{
	// timer is active
	if (IsShieldAnimationFinished == 1) return;
	if (NewMode == EControllerTool::C_None) return;

	/**/ if (NewMode == EControllerTool::C_Gun_Small && !IsGunSmallActive) ControllerMode = EControllerTool::C_Tool;
	else if (NewMode == EControllerTool::C_Gun_Large && !IsGunLargeActive) ControllerMode = EControllerTool::C_Tool;
	else if (NewMode == EControllerTool::C_Gun_Sling && !IsGunSuperActive) ControllerMode = EControllerTool::C_Tool;
	else if (NewMode == EControllerTool::C_Shield    && !IsShieldActive)   ControllerMode = EControllerTool::C_Tool;

	if (ControllerMode == EControllerTool::C_Gun_Sling && NewMode != EControllerTool::C_Gun_Sling) GunSuper->DestroyProjectile();	

	ControllerMode = NewMode;

	// Shield show and hide animation
	if ((ShieldMesh->IsVisible() || ControllerMode == EControllerTool::C_Shield) && IsShieldAnimationFinished == 0) { // timer is inactive
		if (ControllerMode == EControllerTool::C_Shield) {
			if (IsValid(mti_ShieldField)) mti_ShieldField->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
			ShieldMesh->PlayAnimation(anim_ShieldOpen, false);

			// show shield field
			GetWorldTimerManager().SetTimer(hShieldTimer, this, &AHandController::Timer_ReactivateController, 2.0f, false, 2.0f);
		}
		else {
			if (IsValid(mti_ShieldField)) mti_ShieldField->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
			ShieldMesh->PlayAnimation(anim_ShieldClose, false);
			IsShieldAnimationFinished = 1;
			GetWorldTimerManager().SetTimer(hShieldTimer, this, &AHandController::Timer_ReactivateController, 1.2f, false, 1.2f);
			return;
		}
	}
	IsShieldAnimationFinished = 0;

	// show or hide data device
	if (ControllerMode == EControllerTool::C_ModuleRed) {
		SetupVisibleDataDevice(1);
		ControllerMode = EControllerTool::C_Tool;
	}
	else if (ControllerMode == EControllerTool::C_ModuleBlue) {
		SetupVisibleDataDevice(2);
		ControllerMode = EControllerTool::C_Tool;
	}
	else if (ControllerMode == EControllerTool::C_ModuleGreen) {
		SetupVisibleDataDevice(3);
		ControllerMode = EControllerTool::C_Tool;
	}
	else if (ControllerMode == EControllerTool::C_ModuleViolet) {
		SetupVisibleDataDevice(4);
		ControllerMode = EControllerTool::C_Tool;
	}
	else if (ControllerMode == EControllerTool::C_Additional) {
		ControllerMode = EControllerTool::C_Tool;
	}
	else
		SetupVisibleDataDevice(-1);

	if (ControllerMode != EControllerTool::C_Tool) {
		if (IsValid(CurrentBomb)) {
			if (IsValid(PlayerOwner)) {
				CurrentBomb->Throw(PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right));
				CurrentBomb = nullptr;
			}
		}
		else if (IsHoldingStaticMesh()) {
			PutStaticMesh();
		}
	}

	ToolMesh->SetVisibility(ControllerMode == EControllerTool::C_Tool);
	ToolPointer->SetVisibility(false);
	ToolCollider->bGenerateOverlapEvents = false;

	if (GunSmall != nullptr)	GunSmall->SetVisibility(ControllerMode == EControllerTool::C_Gun_Small);
	if (GunLarge != nullptr)	GunLarge->SetVisibility(ControllerMode == EControllerTool::C_Gun_Large);
	if (ShieldMesh != nullptr)	ShieldMesh->SetVisibility(ControllerMode == EControllerTool::C_Shield);
	if (GunSuper != nullptr)	GunSuper->SetWithChildsVisibility(ControllerMode == EControllerTool::C_Gun_Sling);	

	if (ControllerMode == EControllerTool::C_Tool) {
		HitMenuCollider->AttachToComponent(ToolPointer, FAttachmentTransformRules::KeepRelativeTransform, "HitSocket");
		ToolMenuMesh->AttachToComponent(ToolMesh, FAttachmentTransformRules::KeepRelativeTransform, "ToolMenuSocket");
	} else if (ControllerMode == EControllerTool::C_Gun_Small) {
		HitMenuCollider->AttachToComponent(GunSmall, FAttachmentTransformRules::KeepRelativeTransform, "HitSocket");
		ToolMenuMesh->AttachToComponent(GunSmall, FAttachmentTransformRules::KeepRelativeTransform, "ToolMenuSocket");
	} else if (ControllerMode == EControllerTool::C_Gun_Large) {
		HitMenuCollider->AttachToComponent(GunLarge, FAttachmentTransformRules::KeepRelativeTransform, "HitSocket");
		ToolMenuMesh->AttachToComponent(GunLarge, FAttachmentTransformRules::KeepRelativeTransform, "ToolMenuSocket");
	} else if (ControllerMode == EControllerTool::C_Gun_Sling) {
		HitMenuCollider->AttachToComponent(GunSuper, FAttachmentTransformRules::KeepRelativeTransform, "FireSocket");
		ToolMenuMesh->AttachToComponent(GunSuper, FAttachmentTransformRules::KeepRelativeTransform, "ToolMenuSocket");
	} else if (ControllerMode == EControllerTool::C_Shield) {
		HitMenuCollider->AttachToComponent(ShieldMesh, FAttachmentTransformRules::KeepRelativeTransform, "FireSocket");
		ToolMenuMesh->AttachToComponent(ShieldMesh, FAttachmentTransformRules::KeepRelativeTransform, "ToolMenuSocket");
	}
	HitMenuCollider->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	HitMenuCollider->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	if (ControllerMode == EControllerTool::C_Shield)
		ShieldMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	else
		ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHandController::Timer_ReactivateController()
{
	if (ControllerMode == EControllerTool::C_Shield) {
		if (IsValid(mti_ShieldField)) {
			mti_ShieldField->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
		}
	}
	else {
		IsShieldAnimationFinished = 2;
		GetWorldTimerManager().ClearTimer(hShieldTimer);

		UpdateControllerMode(ControllerMode);
	}
}

void AHandController::ThrowIntroLevelBoxCover()
{	
	if (PlayerOwner->CurrentLevelName == "intro") {
		if (IsValid(PlayerOwner->VoicePreset)) {
			if (!PlayerOwner->VoicePreset->IsQuotePlayed(EVoiceQuote::VO_C_Intro_Use_Medpak)) {
				PlayerOwner->PlayVoice(EVoiceQuote::VO_C_Intro_Use_Medpak);
			}
		}
	}
}

FTransform AHandController::GetHandTransform()
{
	if (HandStaticMesh != nullptr)
		return HandStaticMesh->GetComponentTransform();
	else
		return FTransform();
}

FRotator AHandController::GetToolRotation()
{
	if (HandStaticMesh != nullptr)
		return HandStaticMesh->GetComponentRotation();
	else
		return FRotator();
}

AActor * AHandController::GetToolEventRecipient()
{
	return ToolPointer->CurrentEventRecipient;
}

void AHandController::ActoinFire()
{
	if (ControllerMode == EControllerTool::C_Gun_Small) {
		GunSmall->Fire();
	}
	else if (ControllerMode == EControllerTool::C_Gun_Large) {
		GunLarge->Fire();
	}
}

void AHandController::ActionShiftMove(bool Direction)
{
	if (HoldingMesh1 != nullptr) {
		PutStaticMesh();
		return;
	}

	int NewModeToSet = 0;
	bool ClearDataDevices;
	EControllerTool NewMode = EControllerTool::C_Tool;

	int NextDataDevice = -1;
	if (ControllerMode == EControllerTool::C_Tool) {
		if (PlayerOwner != nullptr)
			NextDataDevice = PlayerOwner->GetNextDataDeviceIndex(CurrentDataDevice);
		else
			NextDataDevice = -1;
	}

	switch (ControllerMode) {
		case EControllerTool::C_Tool:	   NewModeToSet = 1; break;
		case EControllerTool::C_Gun_Small: NewModeToSet = 2; break;
		case EControllerTool::C_Gun_Large: NewModeToSet = 3; break;
		case EControllerTool::C_Gun_Sling: NewModeToSet = 4; break;
		case EControllerTool::C_Shield:	   NewModeToSet = 5; break;
	}
	
	bool elementdisabled = true;
	while (elementdisabled) {
		if (Direction) NewModeToSet++; else NewModeToSet--;
		if (NewModeToSet < 1) NewModeToSet = 5;
		if (NewModeToSet > 5) NewModeToSet = 1;

		if (NewModeToSet == 1 && !IsToolActive) elementdisabled = true;
		else if (NewModeToSet == 2 && !IsGunSmallActive) elementdisabled = true;
		else if (NewModeToSet == 3 && !IsGunLargeActive) elementdisabled = true;
		else if (NewModeToSet == 3 && IsValid(AnotherController) && AnotherController->ControllerMode == EControllerTool::C_Gun_Large) elementdisabled = true;
		else if (NewModeToSet == 4 && !IsGunSuperActive) elementdisabled = true;
		else if (NewModeToSet == 4 && IsValid(AnotherController) && AnotherController->ControllerMode == EControllerTool::C_Gun_Sling) elementdisabled = true;
		else if (NewModeToSet == 5 && !IsShieldActive) elementdisabled = true;
		else if (NewModeToSet == 5 && IsValid(AnotherController) && AnotherController->ControllerMode == EControllerTool::C_Shield) elementdisabled = true;
		else elementdisabled = false;
	}

	switch (NewModeToSet) {
		case 1: NewMode = EControllerTool::C_Tool; break;
		case 2: NewMode = EControllerTool::C_Gun_Small; break;
		case 3: NewMode = EControllerTool::C_Gun_Large; break;
		case 4: NewMode = EControllerTool::C_Gun_Sling; break;
		case 5: NewMode = EControllerTool::C_Shield; break;
	}
	ClearDataDevices = (NewMode == EControllerTool::C_Tool && ControllerMode != EControllerTool::C_Tool);
	
	if (ControllerMode == EControllerTool::C_Tool) {
		if (NextDataDevice == -1) {
			UpdateControllerMode(NewMode);
			if (CurrentDataDevice >= 0 && IsValid(PlayerOwner)) {
				PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetVisibility(false);
				PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetProjectorEnabled(false);
				PlayerOwner->SetDataDeviceActive(CurrentDataDevice, -1);
				CurrentDataDevice = -1;
			}
		}
		else if (IsValid(PlayerOwner)) {
			if (CurrentDataDevice >= 0) {
				PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetVisibility(false);
				PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetProjectorEnabled(false);
			}
			PlayerOwner->ControllerDataDevices[NextDataDevice]->SetVisibility(true);
			PlayerOwner->ControllerDataDevices[NextDataDevice]->AttachToController(this);
			PlayerOwner->SetDataDeviceActive(CurrentDataDevice, NextDataDevice);
			CurrentDataDevice = NextDataDevice;

			PlayerOwner->OnDataDeviceSelected(PlayerOwner->ControllerDataDevices[CurrentDataDevice]->UniqueNumber);

		}
	}
	else {
		UpdateControllerMode(NewMode);
	}

	if (ClearDataDevices) CurrentDataDevice = -1;
}

void AHandController::SetupVisibleDataDevice(int32 DataMuduleNumber)
{
	int Index = -1;

	if (DataMuduleNumber > 0) {
		for (int i = 0; i < PlayerOwner->ControllerDataDevices.Num(); i++) {
			if (PlayerOwner->ControllerDataDevices[i]->UniqueNumber == DataMuduleNumber) {
				Index = i; break;
			}
		}
	}

	if (!IsValid(PlayerOwner)) return;
	if (CurrentDataDevice == Index) return; // done already
	if (Index >= PlayerOwner->ControllerDataDevices.Num()) Index = -1;

	// delete device from the other hand if necessary
	if (IsValid(AnotherController) && AnotherController->CurrentDataDevice == Index) {
		AnotherController->SetupVisibleDataDevice(-1);
	}

	// hide active data device
	if (CurrentDataDevice >= 0) {
		PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetVisibility(false);
		PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetProjectorEnabled(false);
	}

	PlayerOwner->SetDataDeviceActive(CurrentDataDevice, Index);

	// show new data device if required
	if (Index >= 0) {
		PlayerOwner->ControllerDataDevices[Index]->SetVisibility(true);
		PlayerOwner->ControllerDataDevices[Index]->AttachToController(this);

		PlayerOwner->OnDataDeviceSelected(PlayerOwner->ControllerDataDevices[Index]->UniqueNumber);
	}

	CurrentDataDevice = Index;
}

void AHandController::SetToolTrace(bool Enabled)
{
	if (Enabled && IsValid(PlayerOwner)) {
		for (int i = 0; i < PlayerOwner->ControllerDataDevices.Num(); i++) {
			if (PlayerOwner->ControllerDataDevices[i]->IsVisible() && PlayerOwner->ControllerDataDevices[i]->IsDeviceAttached() &&
							PlayerOwner->ControllerDataDevices[i]->IsAttachedTo(this)) {
				CurrentDataDevice = i;
				break;
			}
		}
	}
	if (ControllerMode == EControllerTool::C_Tool && (!Enabled || CurrentDataDevice == -1) && !IsHoldingStaticMesh() && 
						(!IsValid(ToolPointer->CurrentEventRecipient) || !ToolPointer->CurrentEventRecipient->IsA(ADataLock::StaticClass())) ) {

		if (IsValid(CurrentJigsaw)) Enabled = false;

		ToolPointer->SetVisibility(Enabled);
		ToolCollider->bGenerateOverlapEvents = Enabled;
		ToolPointer->TracePlayerSelectorLine(0.1f);
	}
	if (IsHoldingStaticMesh() || IsValid(CurrentBomb)) {
		ToolPointer->SetVisibility(false);
		ToolCollider->bGenerateOverlapEvents = false;
	}
}

void AHandController::SetupGunProjectileClass(TSubclassOf<class AUVRPProjectile> ProjClass)
{
	GunSmall->ProjectileClass = ProjClass;
	GunLarge->ProjectileClass = ProjClass;
}

void AHandController::OnBeginOverlap_MenuCollider(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AUVRPCharacter::StaticClass())) return;

	if (OtherComp->ComponentHasTag("TeleportRing") && IsValid(RingsSelectorActor)) {
		ToolPointer->CurrentEventRecipient = OtherActor;
		ATeleport *tel = Cast<ATeleport>(OtherActor);
		if (IsValid(tel) && IsValid(RingsSelectorActor) && tel->IsLockOpen) {
			
			RingsSelectorActor->SetCurrentTeleportMesh(tel->GetTeleportMesh());
			tel->RingSelector = RingsSelectorActor;
			
			RingsSelectorActor->SetRing(3 - tel->RingsStatus);
			RingsSelectorActor->SetVisibility(true);

			//DrawDebugLine(GetWorld(), RingsSelectorActor->GetComponentLocation(), RingsSelectorActor->GetComponentLocation() + FVector(0.0f, 0.0f, 20.0f), FColor::Blue, false, 1.0f);
		}
	}
	else if ((Cast<IMenuInteractionInterface>(OtherActor) != nullptr) || OtherActor->IsA(AMenuCollisionBox::StaticClass())) {
		// is this stuff outdated at all?
		ToolPointer->CurrentEventRecipient = OtherActor;
		/*
		IMenuInteractionInterface *p = Cast<IMenuInteractionInterface>(OtherActor);
		if ((OtherActor->IsA(AMenuCollisionBox::StaticClass())) || p != nullptr) {
			ToolPointer->CurrentEventRecipient = OtherActor;
		}
		*/
	}
	/*
	else if (ToolPointer->IsVisible() && OtherActor->IsA(ADataDevice::StaticClass())) { // pick up with a beam
		ToolPointer->CurrentEventRecipient = OtherActor;
	}
*/
}

void AHandController::OnEndOverlap_MenuCollider(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (IsValid(ToolPointer->CurrentEventRecipient) && OtherActor->GetUniqueID() == ToolPointer->CurrentEventRecipient->GetUniqueID()) ToolPointer->CurrentEventRecipient = nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "out    " + OtherActor->GetName());

	if (OtherComp->ComponentHasTag("TeleportRing") && IsValid(RingsSelectorActor)) {
		RingsSelectorActor->SetVisibility(false);
	}
	if (OtherActor->IsA(ADataDevice::StaticClass())) {
		//BeamDataModule = nullptr;
	}
}

FVector AHandController::GetToolColliderAimTarget()
{
	//ToolPointer->GetComponentTransform()
	FVector vRot = ToolPointer->GetComponentRotation().Vector();
	FVector vSrcPoint = ToolPointer->GetComponentLocation() + vRot * 3.0f;
	FVector vTargetPoint = vSrcPoint + vRot * 100.0f;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace_ToolPointer")), true, this);
	RV_TraceParams.AddIgnoredActor(this);
	RV_TraceParams.AddIgnoredActor(PlayerOwner);
	RV_TraceParams.AddIgnoredComponent(ToolCollider);
	RV_TraceParams.AddIgnoredComponent(ToolPointer);
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(RV_Hit, vSrcPoint, vTargetPoint, ECollisionChannel::ECC_Visibility /* ECC_Pawn*/, RV_TraceParams);

	if (RV_Hit.bBlockingHit && RV_Hit.GetActor() != nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "ok: " +  RV_Hit.GetActor()->GetName() + "   -   " + RV_Hit.ImpactPoint.ToString());
		return RV_Hit.ImpactPoint;
	}
	else {
		/*
		if (RV_Hit.bBlockingHit)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, RV_Hit.GetActor()->GetName());
		else
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "no collision");
		*/
		return FVector::ZeroVector;
	}
}

bool AHandController::TakeStaticMesh(UMeshComponent *MainMesh /* not just Static now */, USceneComponent *AdditionalMesh)
{
	bool bRet = false;
	if (MainMesh != nullptr && HoldingMesh1 == nullptr) {
		if (!MainMesh->IsA(USkeletalMeshComponent::StaticClass())) {
			MainMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		}
		HoldingMesh1 = MainMesh;
		if (!HoldingMesh1->ComponentHasTag(TEXT("STARTFALLING")) && !HoldingMesh1->ComponentHasTag("jigsawpiece")) {
			HoldingMesh1->ComponentTags.Add(TEXT("STARTFALLING"));
		}
		if (AdditionalMesh != nullptr) HoldingMesh2 = AdditionalMesh;
		SetToolTrace(false);

		/////////////////// FEEDBACK ///////////////////
		PlayerOwner->PlaySingleFeedback(EFeedbackAction::A_CatchAndDrop, HandSide == EHandSide::S_Right);
		////////////////////////////////////////////////

		bRet = true;
	}
	
	return bRet;
}

void AHandController::PutStaticMesh()
{
	if (HoldingMesh1 != nullptr) {
		if (IsValid(OnThrowSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnThrowSound, GetActorLocation(), 1, 1, 0, PlayerOwner->GetSoundAttenuation());

		if (IsValid(CurrentBomb) && IsValid(PlayerOwner)) {
			CurrentBomb->Throw(PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right));
			CurrentBomb = nullptr;
			HoldingMesh1 = nullptr;
		}
		else {
			HoldingMesh1->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			HoldingMesh1->SetSimulatePhysics(true);
			HoldingMesh1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			HoldingMesh1->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
			HoldingMesh1->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			if (IsValid(HoldingMesh2)) HoldingMesh2->DestroyComponent();

			if (IsValid(PlayerOwner)) {
				FVector ang = PlayerOwner->GetControllerAngularVelocity(this->HandSide == EHandSide::S_Right);
				FVector lin = PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right);

				HoldingMesh1->SetPhysicsAngularVelocity(ang * 1.5f);
				HoldingMesh1->SetPhysicsLinearVelocity(lin * 1.5f);
			}

			ThrowStaticMesh = HoldingMesh1;
			HoldingMesh1 = nullptr;
			HoldingMesh2 = nullptr;

			ThrowIntroLevelBoxCover();

			if (!hThrowStaticMeshTimer.IsValid()) {
				GetWorldTimerManager().SetTimer(hThrowStaticMeshTimer, this, &AHandController::Timer_ThrowStaticMesh, 1.0f, false);
			}
		}

		/////////////////// FEEDBACK ///////////////////
		PlayerOwner->PlaySingleFeedback(EFeedbackAction::A_CatchAndDrop, HandSide == EHandSide::S_Right);
		////////////////////////////////////////////////
	}
}

void AHandController::Timer_ThrowStaticMesh()
{	
	GetWorldTimerManager().ClearTimer(hThrowStaticMeshTimer);
	hThrowStaticMeshTimer.Invalidate();

	if (IsValid(ThrowStaticMesh)) {
		ThrowStaticMesh->ComponentTags.Remove(TEXT("STARTFALLING"));
	}
}

void AHandController::ReleaseStaticMesh()
{
	if (HoldingMesh1 != nullptr) {
		HoldingMesh1->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		if (IsValid(HoldingMesh2)) HoldingMesh2->DestroyComponent();

		HoldingMesh1 = nullptr;
		HoldingMesh2 = nullptr;
	}

	/////////////////// FEEDBACK ///////////////////
	//PlayerOwner->PlaySingleFeedback(EFeedbackAction::A_CatchAndDrop, HandSide == EHandSide::S_Right);
	////////////////////////////////////////////////
}

bool AHandController::HandleOnClickEvent(EControllerButtonType Button)
{
	bool bRet = true;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "HandleOnClickEvent");

	if (Button == EControllerButtonType::S_Trigger) { // передать сообщение о клике
		if (HoldingMesh1 != nullptr && !HoldingMesh1->ComponentHasTag(TEXT("jigsawpiece"))) {		// выбросить крышку из руки
			PutStaticMesh();
			bRet = false;
		}
		else if (CurrentDataDevice >= 0 && EControllerTool::C_Tool == ControllerMode && Button == EControllerButtonType::S_Trigger) { // включить/выключить флэшку
			if (IsValid(PlayerOwner) && CurrentDataDevice < PlayerOwner->ControllerDataDevices.Num()) {
				PlayerOwner->ControllerDataDevices[CurrentDataDevice]->SetProjectorEnabled(!PlayerOwner->ControllerDataDevices[CurrentDataDevice]->IsProjectorEnabled());
				bRet = false;
			}
		}
		else if (ControllerMode == EControllerTool::C_Tool) {
			FVector point = GetToolColliderAimTarget();
			AActor* recipient = GetToolEventRecipient();
			IMenuInteractionInterface *p = nullptr;
			if (IsValid(recipient)) p = Cast<IMenuInteractionInterface>(recipient);

			/*
			if (IsValid(recipient)) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "HandleOnClickEvent(" + recipient->GetName() + ")");
			} 
			else
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "HandleOnClickEvent(recipient == nullptr)");
			*/

			if (IsValid(recipient) && IsValid(OnClickSound) && !recipient->IsA(AHandMenuSlider::StaticClass())) {
				if (p == nullptr) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnClickSound, recipient->GetActorLocation(), 1, 1, 0, PlayerOwner->GetSoundAttenuation());
				}
				else if (!p->IsUsingCustomSound()) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnClickSound, recipient->GetActorLocation(), 1, 1, 0, PlayerOwner->GetSoundAttenuation());
				}

			}

			if (p != nullptr /*&& ToolPointer->IsVisible()*/) {
				p->OnClick(point);
			}
			else if (IsValid(CurrentBomb) && IsValid(PlayerOwner)) {
				CurrentBomb->Throw(PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right));
				CurrentBomb = nullptr;
			}
			else if (ToolPointer->IsVisible()) {
				// поддержка предыдущей версии кода
				AMenuCollisionBox *colbox = Cast<AMenuCollisionBox>(recipient);
				if (IsValid(colbox)) {
					colbox->SetTerminalLevel();
				}

				// take data device
				if (IsValid(recipient) && recipient->IsA(ADataDevice::StaticClass())) {
					ADataDevice* dm = Cast<ADataDevice>(recipient);

					if (!dm->IsDeviceAttached()) {
						SetToolTrace(false);
						dm->SetProjectorEnabled(false);
						PlayerOwner->TakeDataDeviceAuto(dm, IsRightHand(), true);
					}
				}
			}
		}
		else if (ControllerMode == EControllerTool::C_Gun_Sling) {
			FVector lin = PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right) * 5.5f;
			FireSocketSpeed.Normalize();
			if (FireSocketSpeed.Z < 0.0f && (FMath::Abs(FireSocketSpeed.Z) < FMath::Abs(FireSocketSpeed.X) || FMath::Abs(FireSocketSpeed.Z) < FMath::Abs(FireSocketSpeed.Y)))
				FireSocketSpeed.Z = -FireSocketSpeed.Z;
			GunSuper->Fire(FireSocketSpeed * lin.Size());
			/*
			FVector lin = PlayerOwner->GetControllerVelocity(this->HandSide == EHandSide::S_Right);
			GunSuper->Fire(lin);
			*/
		}
	}
	else if (Button == EControllerButtonType::S_Drop) { // сбросить флэшку, которую держит в руке
		//AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		//gm->ShowMessage(FText::FromString(FString::FromInt(CurrentDataDevice)), 2.0f);
		if (CurrentDataDevice >= 0 && CurrentDataDevice < PlayerOwner->ControllerDataDevices.Num()) {
			if (IsValid(PlayerOwner)) {
				if (PlayerOwner->ControllerDataDevices[CurrentDataDevice]->IsAvalibleForTake && PlayerOwner->ControllerDataDevices[CurrentDataDevice]->CanBeThrownOut) {
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Throw");

					FVector ang = PlayerOwner->GetControllerAngularVelocity(this->HandSide == EHandSide::S_Right) * 1.0f;
					FVector lin = PlayerOwner->GetControllerLinearVelocity(this->HandSide == EHandSide::S_Right) * 1.0f;

					PlayerOwner->ControllerDataDevices[CurrentDataDevice]->ThrowDevice(ang, lin);
					PlayerOwner->PutDataDevice(CurrentDataDevice);
					CurrentDataDevice = -1;

					/////////////////// FEEDBACK ///////////////////
					PlayerOwner->PlaySingleFeedback(EFeedbackAction::A_CatchAndDrop, HandSide == EHandSide::S_Right);
					////////////////////////////////////////////////
				}
			}
		}
		else if (ControllerMode == EControllerTool::C_Gun_Sling) {
			GunSuper->ChangeGunState(EHeavyGunState::C_Bomb, true);
		}
		else if (IsValid(CurrentBomb)) {
			CurrentBomb->SwitchState();
		}
		else if (ControllerMode == EControllerTool::C_Shield && IsAdvancedShieldActive) {
			PlayerOwner->SetTimeWrap();
		}

	}
	
	return bRet;
}

void AHandController::SetTriggerValue(int32 Index, bool Value, AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AHandController::SetTriggerValue (" + OtherActor->GetName() + ", " + OtherComp->GetName() + ")");

	if (Index < 0 || Index > 2) return;
	if (!OtherActor->ActorHasTag(TAG_DataLock) && !OtherComp->ComponentHasTag(TAG_DataLock)) return;

	//if (!OtherActor->IsA(ADataLock::StaticClass())) return;
	/*
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldDynamic)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AHandController::SetTriggerValue (ECC_WorldDynamic)");
	else if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AHandController::SetTriggerValue (ECC_WorldStatic)");
	else if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Pawn)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AHandController::SetTriggerValue (ECC_Pawn)");
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AHandController::SetTriggerValue (unknown)");
	*/

	TriggersState[Index] = Value;

	int tmp = 0;
	if (TriggersState[0]) tmp++;
	if (TriggersState[1]) tmp++;
	if (TriggersState[2]) tmp++;

	if (tmp == 3 && ToolPointer->CurrentEventRecipient == nullptr) {
		ADataLock *lock = Cast<ADataLock>(OtherActor);
		
		bConnectedToLock = (lock != nullptr);

		if (bConnectedToLock) {
			if (lock->ChildDataModule != nullptr && (CurrentDataDevice >= 0 || !lock->IsLockEnabledOnStart)) {		// Флэшка есть и в замке, и в руке - ничего не делать
				return;
			} else if (CurrentDataDevice >= 0) {																	// Сначала вложить флэшку в замок
				if (PlayerOwner && CurrentDataDevice < PlayerOwner->ControllerDataDevices.Num()) {
					if (PlayerOwner->ControllerDataDevices[CurrentDataDevice]->IsAvalibleForTake && 
									PlayerOwner->ControllerDataDevices[CurrentDataDevice]->CanBeThrownOut &&
									!PlayerOwner->ControllerDataDevices[CurrentDataDevice]->IsProjectorEnabled() &&
									lock->CanAcceptDataDevice()) {
						PlayerOwner->ControllerDataDevices[CurrentDataDevice]->AttachToController(lock);
						lock->ChildDataModule = PlayerOwner->ControllerDataDevices[CurrentDataDevice];
						
						/*
						if (IsValid(PlayerOwner->ControllerDataDevices[CurrentDataDevice]))
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "PlayerOwner->ControllerDataDevices[CurrentDataDevice] Is Valid");
						else
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "PlayerOwner->ControllerDataDevices[CurrentDataDevice] Is Not Valid");

						if (IsValid(lock->ChildDataModule))
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "lock->ChildDataModule Is Valid");
						else
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "lock->ChildDataModule Is Not Valid");
						*/

						PlayerOwner->PutDataDevice(CurrentDataDevice);
						CurrentDataDevice = -1;
						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "calling OnDataDeviceAttached()");
						lock->OnDataDeviceAttached(true);
					}
					else return;
				}
			}

			lock->SetControllerConnection(true, this);
		}
		ToolPointer->CurrentEventRecipient = OtherActor;
	}
	else if (tmp < 3 && ToolPointer->CurrentEventRecipient != nullptr) {
		ADataLock *lock = Cast<ADataLock>(ToolPointer->CurrentEventRecipient);
		if (lock != nullptr) {
			lock->SetControllerConnection(false, this);
		}
		ToolPointer->CurrentEventRecipient = nullptr;
	}

	if (tmp < 3) {
		bConnectedToLock = false;
	}
}

void AHandController::OnShieldHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(AUVRPProjectile::StaticClass())) {
		/////////////////// FEEDBACK ///////////////////
		if (IsValid(PlayerOwner)) {
			PlayerOwner->PlaySingleFeedback(EFeedbackAction::A_Shield, HandSide == EHandSide::S_Right);
		}
		////////////////////////////////////////////////
	}
}