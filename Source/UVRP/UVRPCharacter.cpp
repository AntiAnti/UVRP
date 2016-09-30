// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UVRP.h"
#include "Engine.h"
#include "CharacterVoicePreset.h"
#include "Components/ArrowComponent.h"
#include "HandsComponent.h"
#include "HandController.h"
#include "TeleportBase.h"
#include "UVRPProjectile.h"
#include "UVRPGameMode.h"
#include "DrawDebugHelpers.h"
#include "HeadMountedDisplay.h"
#include "MotionControllerComponent.h"
#include "UVRPPlayerController.h"
#include "DataDevice.h"
#include "MenuInteractionInterface.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "TeleportBase.h"
#include "SteamVRChaperoneComponent.h"

#include "UVRPCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AUVRPCharacter

AUVRPCharacter::AUVRPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	Health = 1.0f;
	CurrentPlatformName = "";

	CameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("CameraBase"));
	CameraBase->SetupAttachment(GetCapsuleComponent());
	CameraBase->RelativeLocation = FVector::ZeroVector;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(CameraBase);
	FirstPersonCameraComponent->RelativeLocation = FVector::ZeroVector;

	// Default offset from the character location for projectiles to spawn
	//GunOffset = FVector(100.0f, 33.0f, 10.0f);
	GunOffset = FVector(20.0f, 20.0f, -1500.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<UHandsComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	GunArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("GunArrow"));
	GunArrow->SetupAttachment(GetCapsuleComponent());
	GunArrow->RelativeLocation = FVector(10.0f, 0.0f, 64.0f); // Position the camera

	LeftHandDisplay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandDisplay"));
	LeftHandDisplay->SetupAttachment(Mesh1P, TEXT("DisplayLeft_Menu"));
	LeftHandDisplay->SetRelativeLocation(FVector::ZeroVector);
	LeftHandDisplay->SetRelativeRotation(FRotator::ZeroRotator);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smHandDisplay(TEXT("StaticMesh'/Game/FirstPerson/Objects/Hands/sm_PlayerHandDIsplay.sm_PlayerHandDIsplay'"));
	if (smHandDisplay.Object != NULL) {
		LeftHandDisplay->SetStaticMesh((UStaticMesh*)smHandDisplay.Object);

		static ConstructorHelpers::FObjectFinder<UMaterialInstance> smHandDisplayMaterial(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_Menu_ProgressBar.mti_Menu_ProgressBar'"));
		if (smHandDisplayMaterial.Object != NULL) {
			LeftHandDisplay->SetMaterial(2, (UMaterialInstance*)smHandDisplayMaterial.Object);
		}
	}

	SteamVRChaperone = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("SteamVRChaperone"));
	if (SteamVRChaperone) {
		//SteamVRChaperone->
		//CharacterMovement->UpdatedComponent = CapsuleComponent;
	}

	IsGamePaused = true;
	PlayerBaseOffset = FVector::ZeroVector;
	CurrentLevelName = "menu";

	static ConstructorHelpers::FObjectFinder<UReverbEffect> aeTubeReverb(TEXT("ReverbEffect'/Game/FirstPerson/Audio/TubeVoiceReverb.TubeVoiceReverb'"));
	if (aeTubeReverb.Object != NULL) {
		TubeEchoEffect = (UReverbEffect*)aeTubeReverb.Object;
	}

	PlayVoiceComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayAudioComp"));
	if (PlayVoiceComponent) {
		PlayVoiceComponent->SetupAttachment(GetRootComponent());
		PlayVoiceComponent->bStopWhenOwnerDestroyed = true;
	}

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> aeTubeEffect(TEXT("SoundAttenuation'/Game/FirstPerson/Audio/TubeVoiceEffects.TubeVoiceEffects'"));
	if (aeTubeEffect.Object != NULL) {
		SoundEffects = (USoundAttenuation*)aeTubeEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> sbTimeWrap(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/timewrap_2.timewrap_2'"));
	if (sbTimeWrap.Object != NULL) {
		TimeWrapEffect = (USoundBase*)sbTimeWrap.Object;
	}
	
	/*
	ControllersRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ControllersRoot"));
	ControllersRoot->AttachParent = RootComponent;
	ControllersRoot->SetRelativeLocation(FVector(65.0f, 0.0f, 15.0f));

	MControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MControllerLeft"));
	MControllerLeft->SetRelativeLocation(FVector(65.0f, 0.0f, 15.0f));
	MControllerLeft->AttachParent = ControllersRoot;
	//MControllerLeft->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MControllerLeft->Hand = EControllerHand::Left;

	MControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MControllerRight"));
	MControllerRight->SetRelativeLocation(FVector(65.0f, 0.0f, 15.0f));
	MControllerRight->AttachParent = ControllersRoot;
	//MControllerRight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MControllerRight->Hand = EControllerHand::Right;

	MHandToolLeft = CreateDefaultSubobject<UChildActorComponent>(TEXT("MHandToolLeft"));
	MHandToolLeft->SetChildActorClass(AHandController::StaticClass());
	MHandToolLeft->CreateChildActor();
	MHandToolLeft->SetRelativeLocation(FVector(65.0f, 0.0f, 15.0f));
	MHandToolLeft->AttachParent = ControllersRoot;
	MHandToolLeft->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MHandToolLeft->AttachParent = MControllerLeft;
	LeftHandToolController = Cast<AHandController>(MHandToolLeft->ChildActor);
	if (LeftHandToolController != nullptr) {
		LeftHandToolController->SetHandSide(EHandSide::S_Left);
		LeftHandToolController->SetupGunProjectileClass(ProjectileClass);
	}
	MHandToolLeft->RegisterComponent();

	MHandToolRight = CreateDefaultSubobject<UChildActorComponent>(TEXT("MHandToolRight"));
	MHandToolRight->SetChildActorClass(AHandController::StaticClass());
	MHandToolRight->CreateChildActor();
	MHandToolRight->SetRelativeLocation(FVector(65.0f, 0.0f, 15.0f));
	MHandToolRight->AttachParent = ControllersRoot;
	MHandToolRight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MHandToolRight->AttachParent = MControllerRight;
	RightHandToolController = Cast<AHandController>(MHandToolRight->ChildActor);
	if (RightHandToolController != nullptr) {
		RightHandToolController->SetHandSide(EHandSide::S_Right);
		RightHandToolController->SetupGunProjectileClass(ProjectileClass);
	}
	MHandToolRight->RegisterComponent();
	*/

	DamageFadeInTime = 0.2f;
	DamageFadeOutTime = 0.6f;
	DamageAlphaMultiplier = 0.03f;
	DamageMultiplier = 1.0f;
	TimeWrapMultiplier = 1.0f;

	HeadOffsetTime = 0.0f;
	HeadOffsetValue = 0.0f;
	InitialHealth = 1.0f;
	IsReturningFromMenu = false;
	IsGamePaused = true;
	CanShowTeleportPortal = true;
	BGMusicParams.VolumeName = "nomusic";
	CanOpenMainMenu = true;
	IsInTrainingLevel = false;
}

void AUVRPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AUVRPCharacter::BeginPlay()
{
	Super::BeginPlay();

	// materials init
	if (!IsValid(mti_HandDisplay)) {
		mti_HandDisplay = LeftHandDisplay->CreateDynamicMaterialInstance(2);
	}

	if (IsValid(mti_HandDisplay)) {
		mti_HandDisplay->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor(0.91f, 0.513f, 0.0f, 1.0f));
		mti_HandDisplay->SetScalarParameterValue(TEXT("Value"), Health);
	}

	// controllers
	if (IsValid(LeftHandToolController) && IsValid(RightHandToolController)) {
		LeftHandToolController->SetAnotherController(RightHandToolController);
		RightHandToolController->SetAnotherController(LeftHandToolController);
	}

	// update hands mesh
	AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(gm)) {
		UpdateHandsView(gm->GO_ShowHands);
		ActiveGameMode = gm;
	}

	/*
	if (LeftHandToolController == nullptr) {
		LeftHandToolController = Cast<AHandController>(MHandToolLeft->ChildActor);
		if (LeftHandToolController != nullptr) {
			LeftHandToolController->SetHandSide(EHandSide::S_Left);
			LeftHandToolController->SetupGunProjectileClass(ProjectileClass);
		}
	}
	if (RightHandToolController == nullptr) {
		RightHandToolController = Cast<AHandController>(MHandToolRight->ChildActor);
		if (RightHandToolController != nullptr) {
			RightHandToolController->SetHandSide(EHandSide::S_Right);
			RightHandToolController->SetupGunProjectileClass(ProjectileClass);
		}
	}
	if (LeftHandToolController != nullptr) {
		LeftHandToolController->AttachRootComponentToActor(this);
		LeftHandToolController->AttachRootComponentTo(MControllerLeft);
		//LeftHandToolController->SetActorRelativeLocation(FVector::ZeroVector);
		//LeftHandToolController->SetActorRelativeRotation(FRotator().Quaternion());
		LeftHandToolController->SetHandSide(EHandSide::S_Left);
		LeftHandToolController->SetupGunProjectileClass(ProjectileClass);
	}

	if (RightHandToolController != nullptr) {
		RightHandToolController->AttachRootComponentToActor(this);
		RightHandToolController->AttachRootComponentTo(MControllerRight);
		//RightHandToolController->SetActorRelativeLocation(FVector::ZeroVector);
		//RightHandToolController->SetActorRelativeRotation(FRotator().Quaternion());
		RightHandToolController->SetHandSide(EHandSide::S_Right);
		RightHandToolController->SetupGunProjectileClass(ProjectileClass);
	}
	*/
	Heal(1.0f);
}

void AUVRPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform HandTransf;
	FRotator ToolRot;

	/*
	if (Mesh1P->IsObjectRotating()) {
		// true if player's body is rotating

		FRotator rot;
		FVector pos;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(rot, pos);

		if (rot.Yaw != 0.0f || rot.Pitch != 0.0f) {
			FRotator rotCtrl = Mesh1P->GetComponentRotation();
			rotCtrl.Yaw = rot.Yaw;
			//Mesh1P->SetWorldRotation(rotCtrl);
		}
	}
	*/

	/**************************************************************************************************************/
	/*********************************************** обновление рук ***********************************************/
	/**************************************************************************************************************/
	if (RightHandToolController != nullptr) {
		HandTransf = RightHandToolController->GetHandTransform();
		Mesh1P->RightHandLocation = HandTransf.GetLocation();
		Mesh1P->RightHandRotation = HandTransf.GetRotation().Rotator();

		ToolRot = RightHandToolController->GetToolRotation();
		//if (Mesh1P->RightToolRotation.Roll < -170.0f && ToolRot.Roll > 140.0f) ToolRot.Roll = 360.0f - ToolRot.Roll;
		Mesh1P->RightToolRotation = ToolRot; // RightHandToolController->GetToolRotation();
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, Mesh1P->RightHandLocation.ToString());
		//DrawDebugLine(this->GetWorld(), HandTransf.GetLocation(), HandTransf.GetLocation() + HandTransf.GetRotation().Rotator().Vector()*100.0f, FColor::White, false, 0.1f, 0, 5.0f);
	}
	if (LeftHandToolController != nullptr) {
		HandTransf = LeftHandToolController->GetHandTransform();
		Mesh1P->LeftHandLocation = HandTransf.GetLocation();
		Mesh1P->LeftHandRotation = HandTransf.GetRotation().Rotator();

		ToolRot = LeftHandToolController->GetToolRotation();
		//if (Mesh1P->LeftToolRotation.Roll < -170.0f && ToolRot.Roll > 140.0f) ToolRot.Roll = 360.0f - ToolRot.Roll;
		Mesh1P->LeftToolRotation = LeftHandToolController->GetToolRotation();
		//DrawDebugLine(this->GetWorld(), HandTransf.GetLocation(), HandTransf.GetLocation() + HandTransf.GetRotation().Rotator().Vector()*100.0f, FColor::White, false, 0.1f, 0, 5.0f);
	}
	if (RightHandToolController != nullptr || LeftHandToolController != nullptr)
		Mesh1P->UpdateHandsMesh();

	/**************************************************************************************************************/
	/*********************************************** видимый портал ***********************************************/
	/**************************************************************************************************************/
	if (IsPortalVisible) {
		FRotator newrot;
		float DeltaYaw, z;
		newrot = UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComponent->GetComponentLocation(), PortalCaptureTarget->GetActorLocation());

		DeltaYaw = FMath::Abs(newrot.Yaw - PortalCaptureCamera->GetActorRotation().Yaw);
		z = PortalCaptureCamera->GetActorLocation().Z;

		FVector addvec = TargetPortalPlatform->GetActorLocation() + newrot.Vector() * 35.0f;
		addvec.Z = z;
		PortalCaptureCamera->SetActorLocation(addvec);

		PortalCaptureCamera->SetActorRotation(newrot);
		PortalCaptureTarget->SetActorRotation(newrot);
	}
	/**************************************************************************************************************/
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUVRPCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	// --------------------- don't bind mouse & keyboard locomotion input for HMD --------------------- 
	if (__IS_BUILD_FINAL || UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) return;
	// ------------------------------------------------------------------------------------------------

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump).bExecuteWhenPaused = true;
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping).bExecuteWhenPaused = true;
	InputComponent->BindAction("Fire", IE_Pressed, this, &AUVRPCharacter::OnFire);
	
	InputComponent->BindAxis("MoveForward", this, &AUVRPCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUVRPCharacter::MoveRight).bExecuteWhenPaused = true;	
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput).bExecuteWhenPaused = true;
	InputComponent->BindAxis("TurnRate", this, &AUVRPCharacter::TurnAtRate).bExecuteWhenPaused = true;
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput).bExecuteWhenPaused = true;
	InputComponent->BindAxis("LookUpRate", this, &AUVRPCharacter::LookUpAtRate).bExecuteWhenPaused = true;
}

void AUVRPCharacter::ResetControllerOrientation()
{	
	if (false && GEngine->HMDDevice.IsValid()) {
		FVector pos;
		FRotator rot, rotPawn;

		rotPawn = GetController()->GetControlRotation();

		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(rot, pos);		

		rotPawn.Yaw += rot.Yaw;
		GetController()->SetControlRotation(rot);
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(rot.Yaw, EOrientPositionSelector::Type::Orientation);
		/*
		rotPawn = this->GetActorRotation();
		rotPawn.Yaw = rot.Yaw;
		this->SetActorRotation(rotPawn);
		*/
	}
}

void AUVRPCharacter::ResetMenuReturningFlag()
{
	IsReturningFromMenu = false;
	GetWorldTimerManager().ClearTimer(TimerReturnFromMenu);
}

void AUVRPCharacter::TranslateMenuInterfaceEvent(AActor *Target, FVector HitPoint)
{
	IMenuInteractionInterface *p = Cast<IMenuInteractionInterface>(Target);
	if (p != nullptr) p->OnClick(HitPoint);
}

void AUVRPCharacter::OnFire()
{ 
}

void AUVRPCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AUVRPCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AUVRPCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AUVRPCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		ResetControllerOrientation();

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUVRPCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		ResetControllerOrientation();

		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUVRPCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUVRPCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AUVRPCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUVRPCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AUVRPCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUVRPCharacter::TouchUpdate);
	}
	return bResult;
}

void AUVRPCharacter::TakeDataDevice(ADataDevice *NewDevice)
{
	ControllerDataDevices.Add(NewDevice);
	ControllerDataDevicesAvailability.Add(true);
}

bool AUVRPCharacter::PutDataDevice(int32 Index)
{
	bool bRet = false;
	if (Index >= 0 && Index < ControllerDataDevices.Num()) {
		AHandController* OtherCtrl = nullptr;
		int32 ReplaceId = -1;
		int32 OldIndex = -1;
		int32 NewIndex = -1;
		
		if (RightHandToolController->CurrentDataDevice == Index) {
			OtherCtrl = LeftHandToolController;
		}
		else if (LeftHandToolController->CurrentDataDevice == Index) {
			OtherCtrl = RightHandToolController;
		}
		else {
			ActiveGameMode->ShowMessage(FText::FromString("Error: Can't get data module attachment parent"), 4.0f);
		}
		if (IsValid(OtherCtrl) && OtherCtrl->CurrentDataDevice >= 0 && OtherCtrl->CurrentDataDevice <= ControllerDataDevices.Num()) {
			ReplaceId = ControllerDataDevices[OtherCtrl->CurrentDataDevice]->UniqueNumber;
			ControllerDataDevicesAvailability[OtherCtrl->CurrentDataDevice] = true;
		}

		ControllerDataDevices.RemoveAt(Index);
		ControllerDataDevicesAvailability.RemoveAt(Index);

		if (ReplaceId >= 0) {
			for (int i = 0; i < ControllerDataDevices.Num(); i++) {
				if (ControllerDataDevices[i]->UniqueNumber == ReplaceId) {
					NewIndex = i;
					OtherCtrl->CurrentDataDevice = NewIndex;
					ControllerDataDevicesAvailability[NewIndex] = false;
					break;
				}
			}
		}

		bRet = true;
	}

	return bRet;
}

void AUVRPCharacter::TakeDataDeviceAuto(ADataDevice *NewDevice, bool RightHand, bool KeepDeviceVisible)
{
	AHandController* ctrl = RightHand ? RightHandToolController : LeftHandToolController;

	if (IsValid(ctrl)) {
		NewDevice->AttachToController(ctrl);
		TakeDataDevice(NewDevice);

		// hide data module - default option
		if (!KeepDeviceVisible) {
			ctrl->CurrentDataDevice = -1; // ControllerDataDevices.Num() - 1;
			NewDevice->SetVisibility(false);
			SetDataDeviceActive(ControllerDataDevices.Num() - 1, -1);
		}
		else {
			ctrl->CurrentDataDevice = ControllerDataDevices.Num() - 1;
		}
	}
}


void AUVRPCharacter::SetDataDeviceActive(int32 CurrentIndex, int32 NewIndex)
{
	if (NewIndex >= 0) ControllerDataDevicesAvailability[NewIndex] = true;
	if (CurrentIndex >= 0) ControllerDataDevicesAvailability[CurrentIndex] = false;
}

int32 AUVRPCharacter::GetNextDataDeviceIndex(int32 CurrentIndex)
{
	int ind = CurrentIndex + 1;
	while (ind != ControllerDataDevices.Num() && ControllerDataDevicesAvailability[ind]) {
		ind++;
	}
	if (ind == ControllerDataDevices.Num()) ind = -1;

	return ind;
}

FVector AUVRPCharacter::GetHeadLocation()
{
	return FirstPersonCameraComponent->GetComponentLocation();
}

FVector AUVRPCharacter::GetFootLocation()
{
	FVector ret = FirstPersonCameraComponent->GetComponentLocation();
	ret.Z = GetCapsuleComponent()->GetComponentLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	return ret;
}

FVector AUVRPCharacter::GetLeftHandLocation()
{
	FVector ret = GetCapsuleComponent()->GetComponentLocation();
	float radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float AngleYaw = FMath::DegreesToRadians(FirstPersonCameraComponent->GetComponentRotation().Yaw + 90.0f);
	ret.X += FMath::Cos(AngleYaw) * radius;
	ret.Y += FMath::Sin(AngleYaw) * radius;

	return ret;
}

FVector AUVRPCharacter::GetRightHandLocation()
{
	FVector ret = GetCapsuleComponent()->GetComponentLocation();
	float radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float AngleYaw = FMath::DegreesToRadians(FirstPersonCameraComponent->GetComponentRotation().Yaw - 90.0f);
	ret.X += FMath::Cos(AngleYaw) * radius;
	ret.Y += FMath::Sin(AngleYaw) * radius;

	return ret;
}

void AUVRPCharacter::Damage(float Value)
{
	bool bGodMode;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Damage Received: " + FString::SanitizeFloat(Value));

	if (IsValid(ActiveGameMode)) bGodMode = ActiveGameMode->GetGodMode(); else bGodMode = false;

	if (!bGodMode || Value == 100.0f) Health -= Value * DamageMultiplier;
	if (Health <= 0.0f) Health = 0.0f;
	if (!IsValid(mti_HandDisplay)) {
		mti_HandDisplay = LeftHandDisplay->CreateDynamicMaterialInstance(2);
		if (IsValid(mti_HandDisplay)) mti_HandDisplay->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor(0.91f, 0.513f, 0.0f, 1.0f));
	}
	if (IsValid(mti_HandDisplay)) mti_HandDisplay->SetScalarParameterValue(TEXT("Value"), Health);

	// player dead
	if (Health <= 0.0f) {
		Health = 0.0f;

		// condition to ignore training level
		if (GetIsGameActive()) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "I'm dead man");
			SetIsGameActive(false);
			PlayVoiceComponent->Stop();
			if (IsValid(AI_VoicePreset)) {
				AI_VoicePreset->StopPlaying();
				AI_VoicePreset->PlayElementByQuote(EVoiceQuote::VQ_C_Death);
			}
			if (IsValid(ActiveGameMode)) ActiveGameMode->GameStat.Died++;
			ActiveGameMode->SaveGameOptions();
			ActiveGameMode->SaveGameStatistics();

			if (!hTimerKillPlayer.IsValid()) {
				GetWorldTimerManager().SetTimer(hTimerKillPlayer, this, &AUVRPCharacter::KillPlayer, 2.0f, false);
			}
		}
	}
	else if (!bGodMode || Value == 100.0f) {
		// don't broadcast event if player is dead - we don't need to show damage fade
		// it's better to make a check in DamageReceived() definition
		DamageReceived(Value);
	}
}

void AUVRPCharacter::KillPlayer()
{
	IsGamePaused = false;

	GetWorldTimerManager().ClearTimer(hTimerKillPlayer);
	hTimerKillPlayer.Invalidate();

	// blueprint event
	PlayerDied();

	OpenMainMenu();
}

void AUVRPCharacter::Heal(float Value)
{
	Health += Value * DamageMultiplier;
	if (Health > 1.0f) Health = 1.0f;

	if (IsValid(mti_HandDisplay)) mti_HandDisplay->SetScalarParameterValue(TEXT("Value"), Health);
}

void AUVRPCharacter::PlayerDied_Implementation()
{
	//
	return;
}

void AUVRPCharacter::SetTimeWrap()
{
	if (TimeWrapMultiplier == 1.0f && !hTimeWrapTimer.IsValid()) {
		TimeWrapMultiplier = 0.3f;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeWrapMultiplier);

		this->CustomTimeDilation = 1.0f;
		GetWorldTimerManager().SetTimer(hTimeWrapTimer, this, &AUVRPCharacter::SetTimeWrap, 5.0f * TimeWrapMultiplier, false);
		if (IsValid(TimeWrapEffect)) UGameplayStatics::PlaySound2D(GetWorld(), TimeWrapEffect, 0.5f);
	}
	else {
		TimeWrapMultiplier = 1.0f;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeWrapMultiplier);
		GetWorldTimerManager().ClearTimer(hTimeWrapTimer);
		hTimeWrapTimer.Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// GLOBAL GAME CONTROL FUNCTIONS ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool AUVRPCharacter::TeleportToLevel(FString LevelName)
{
	FVector MoveTo;
	FString PSName;
	FVector vOffset = FVector::ZeroVector;
	bool bRet = false;

	LevelName = LevelName.ToLower();
	CurrentLevelName = LevelName;
	IsGamePaused = (LevelName == "menu");

	if (IsInTrainingLevel && IsValid(ActiveGameMode)) {
		if (GameScore > ActiveGameMode->GP_TrainingLevelScore) {
			ActiveGameMode->GP_TrainingLevelScore = GameScore;
		}
	}
	IsInTrainingLevel = (LevelName == "trainingroom");
	
	if (LevelName == "menu")
		PSName = "PlayerStart_LevelMenu";
	else if(LevelName == "main")
		PSName = "PlayerStart_LevelMain";
	else if (LevelName == "intro")
		PSName = "PlayerStart_LevelIntro";
	else if (LevelName == "trainingroom") {
		PSName = "PlayerStart_LevelTraining";		
	}
	else if (LevelName == "titles") {
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("titles"));
		return true;
	}
	else if (LevelName == "demolevel") {
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("demolevel"));
		return true;
	}
	else if (LevelName == "game") {
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("game"));
		IsGamePaused = true;
		return true;
	}

	if (IsValid(ActiveGameMode)) {
		vOffset.Z = ActiveGameMode->GO_FloorOffset;
	}

	if (LevelName != "menu") IsGamePaused = false;

	// echo
	if (CurrentLevelName == "main" && TubeEchoEffect != nullptr)
		UGameplayStatics::ActivateReverbEffect(GetWorld(), TubeEchoEffect, TEXT("TubeReverb"));
	else
		UGameplayStatics::DeactivateReverbEffect(GetWorld(), TEXT("TubeReverb"));

	FString a;
	// Find player start location
	for (TActorIterator<APlayerStart> itPS(GetWorld()); itPS; ++itPS)
	{
		a = itPS->GetName();
		if (a == PSName) {
			if (CurrentLevelName == "main")
				SetActorLocation(itPS->GetActorLocation() + CalculateMainLevelOffest() + vOffset);
			else
				SetActorLocation(itPS->GetActorLocation() + vOffset);

			bRet = true;
			break;
		}
	}
	if (!bRet && CurrentLevelName == "main") {
		SetActorLocation(FVector(-475.58f, 185.0f, -818.0f) + CalculateMainLevelOffest() + vOffset);
		bRet = true;
	}

	if (CurrentLevelName == "main") {
		CameFormInterLevel();
	}

	return bRet;
}

void AUVRPCharacter::OpenMainMenu()
{
	const FVector StartLocation = FVector(5000.0f, 70.0f, 76.0f);
	FVector vOffset = FVector::ZeroVector;
	FString lvl = UGameplayStatics::GetCurrentLevelName(GetWorld());

	if (!IsValid(ActiveGameMode)) ActiveGameMode = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (IsValid(ActiveGameMode)) {
		vOffset.Z = ActiveGameMode->GO_FloorOffset;
	}
	/*
	if (IsGamePaused)
		ActiveGameMode->ShowMessage(FText::FromString("IsGamePaused == true"));
	if (!GetIsGameActive())
		ActiveGameMode->ShowMessage(FText::FromString("GetIsGameActive() == false"));
	if (!CanOpenMainMenu)
		ActiveGameMode->ShowMessage(FText::FromString("CanOpenMainMenu == false"));
	*/

	if (lvl != "titles" && lvl != "trainingroom" && lvl != "demolevel") {
		if (IsGamePaused || (GetIsGameActive() && !CanOpenMainMenu)) return;
	}
	PlayerBaseOffset = GetActorLocation();
	if (PlayerBaseOffset.X > 9500.0f && PlayerBaseOffset.Z < 1270.0f) return; // secret area 2

	//UGameplayStatics::SetGamePaused(GetWorld(), IsGamePaused);


	/*
	if (CurrentLocationBase != nullptr)
		PlayerBaseOffset = GetActorLocation() - CurrentLocationBase->GetActorLocation();
	else
		PlayerBaseOffset = FVector::ZeroVector;
	PlayerBaseOffset.Z = 0.0f;
	*/

	if ((FirstPersonCameraComponent->GetComponentLocation() - PlayerBaseOffset).Size() > 150.0f) {
		if (IsValid(ActiveGameMode)) {
			ActiveGameMode->ShowMessage(FText::FromString("Can't open main menu. Step closer to the current\nteleport or platform centre!"), 10.0f);
		}
	}
	else {
		if (!GetIsGameActive() || lvl == "titles" || lvl == "trainingroom" || lvl == "demolevel" || IsInTrainingLevel) {
			if (IsInTrainingLevel && IsValid(ActiveGameMode)) {
				if (GameScore > ActiveGameMode->GP_TrainingLevelScore) {
					ActiveGameMode->GP_TrainingLevelScore = GameScore;
				}
			}
			IsInTrainingLevel = false;
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("game"));
			return;
		}

		IsInTrainingLevel = false;
		IsGamePaused = true;
		UGameplayStatics::DeactivateReverbEffect(GetWorld(), TEXT("TubeReverb"));
		SetActorLocation(StartLocation /*+ PlayerBaseOffset*/ + vOffset);
		PlayVoiceComponent->Deactivate();

		if (IsValid(BGAudioComp) && BGAudioComp->IsPlaying()) {
			BGAudioComp->bAutoDestroy = false;
			BGAudioComp->Stop();
			BGAudioPlayingPosition = GetWorld()->GetRealTimeSeconds() - BGAudioPlayingStartTime;
		}
		else BGAudioPlayingPosition = 0.0f;

		//if (PlayVoiceComponent->IsPlaying()) PlayVoiceComponent->Stop();
		//PlayVoiceComponent->Play()
	}

	SetupGameplayTime(false);
}

void AUVRPCharacter::ReturnFromMainMenu()
{
	if (!IsGamePaused || CurrentLevelName == "menu") return;
	const FVector StartLocation = FVector(5000.0f, 70.0f, 82.0f);

	/*
	if (CurrentLocationBase != nullptr)
		PlayerBaseOffset = GetActorLocation(); // -StartLocation;
	else
		PlayerBaseOffset = FVector::ZeroVector;
	PlayerBaseOffset.Z = 0.0f;
	*/

	if (CurrentLevelName == "main" && TubeEchoEffect != nullptr)
		UGameplayStatics::ActivateReverbEffect(GetWorld(), TubeEchoEffect, TEXT("TubeReverb"));

	if (CurrentLocationBase != nullptr) {
		if ((FirstPersonCameraComponent->GetComponentLocation() - GetActorLocation()).Size() > 150.0f) {
			// заменить на окно с предупреждением!
			if (IsValid(ActiveGameMode)) {
				ActiveGameMode->ShowMessage(FText::FromString("Can't open game. Step closer to the current\nteleport or platform centre!"), 10.0f);
			}
		}
		else {
			IsReturningFromMenu = true;
			SetActorLocation(/*CurrentLocationBase->GetActorLocation() + */PlayerBaseOffset);
			PlayVoiceComponent->Activate();
			IsGamePaused = false;
			if (IsValid(BGAudioComp) && BGAudioPlayingPosition > 0) BGAudioComp->Play(BGAudioPlayingPosition);
			/*
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::SanitizeFloat(BGAudioPlayingPosition));
				if (IsValid(BGAudioComp))
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "BGAudioComp is valid");
				else
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "BGAudioComp is not valid");
			}*/

			GetWorldTimerManager().SetTimer(TimerReturnFromMenu, 0.5f, false, 0.5f);
		}
		//UGameplayStatics::SetGamePaused(GetWorld(), IsGamePaused);
	}	
	SetupGameplayTime(true);
}

float AUVRPCharacter::PlayVoice(EVoiceQuote quote, bool force)
{
	float fRet = 0.0f;
	/*
	FString PlayingFileName;

	if (AI_VoicePreset) {
		USoundBase* sound = AI_VoicePreset->GetRandomElement(quote);
		if (sound) {
			if (PlayVoiceComponent->IsPlaying())
				PlayVoiceComponent->Stop();

			PlayVoiceComponent->SetSound(sound);
			PlayVoiceComponent->Play();			
		}

		fRet = sound->Duration;
	}
	*/

	if (IsValid(VoicePreset)) fRet = VoicePreset->PlayElementByQuote(quote, force);

	return fRet;
}

void AUVRPCharacter::AttachActorToHand_Implementation(AActor *MenuActor, FName SocketName)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "AUVRPCharacter::AttachActorToHand(...) implementation");
	if (MenuActor) {
		MenuActor->AttachToComponent(Mesh1P, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	}
}

void AUVRPCharacter::SetGunSmallActive(bool IsActive)
{
	if (IsValid(LeftHandToolController)) LeftHandToolController->IsGunSmallActive = IsActive;
	if (IsValid(RightHandToolController)) RightHandToolController->IsGunSmallActive = IsActive;
}

void AUVRPCharacter::SetGunLargeActive(bool IsActive)
{
	if (IsValid(LeftHandToolController)) LeftHandToolController->IsGunLargeActive = IsActive;
	if (IsValid(RightHandToolController)) RightHandToolController->IsGunLargeActive = IsActive;
}

void AUVRPCharacter::SetGunSlingActive(bool IsActive)
{
	if (IsValid(LeftHandToolController)) LeftHandToolController->IsGunSuperActive = IsActive;
	if (IsValid(RightHandToolController)) RightHandToolController->IsGunSuperActive = IsActive;

	////////////////////////////////////////////////////////////////////////////////////////
	if (IsValid(ActiveGameMode) && IsActive && ActiveGameMode->IsLoadingGame == false)
		PlayVoice(EVoiceQuote::VO_C_Gl_Harpoon1);
	////////////////////////////////////////////////////////////////////////////////////////
}

void AUVRPCharacter::SetShieldActive(bool IsActive)
{
	if (IsValid(LeftHandToolController)) LeftHandToolController->IsShieldActive = IsActive;
	if (IsValid(RightHandToolController)) RightHandToolController->IsShieldActive = IsActive;

	if (IsActive && IsValid(ActiveGameMode) && !ActiveGameMode->IsLoadingGame) {
		FString lvl = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (lvl != TEXT("demolevel")) {
			ActiveGameMode->ShowMessage(FText::FromString("Good work! You can use hand shield now."), 10.0f);
		}
	}
}

void AUVRPCharacter::SetAdvancedShieldActive(bool IsActive)
{
	const FLinearColor colorActive = FLinearColor(0.232f, 0.432f, 0.93f, 1.0f);
	const FLinearColor colorPassive = FLinearColor(0.93f, 0.4f, 0.14f, 1.0f);
	/***/ FLinearColor color = IsActive ? colorActive : colorPassive;

	if (IsValid(LeftHandToolController)) {
		LeftHandToolController->IsAdvancedShieldActive = IsActive;
		LeftHandToolController->SetShieldColor(color);
	}
	if (IsValid(RightHandToolController)) {
		RightHandToolController->IsAdvancedShieldActive = IsActive;
		RightHandToolController->SetShieldColor(color);
	}
}

FRotator AUVRPCharacter::GetViewRotation() const
{
	if (AUVRPPlayerController* MYPC = Cast<AUVRPPlayerController>(Controller)) {
		return MYPC->GetViewRotation();
	}
	else if (Role < ROLE_Authority) {
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)	{
			APlayerController* PlayerController = *Iterator;
			if (PlayerController && PlayerController->PlayerCameraManager->GetViewTargetPawn() == this) 	{
				return PlayerController->BlendedTargetViewRotation;
			}
		}
	}

	if (IsValid(GetController()))
		return GetController()->GetControlRotation();
	else
		return FRotator();
}

AHandController* AUVRPCharacter::GetRightController()
{
	return RightHandToolController;
}

AHandController* AUVRPCharacter::GetLeftController()
{
	return LeftHandToolController;
}

void AUVRPCharacter::SetHandMeshVisibility(bool RightHand, bool IsVisible)
{
	if (RightHand && IsValid(RightHandToolController)) {
		RightHandToolController->SetHandVisibility(IsVisible);
	}
	else if (!RightHand && IsValid(LeftHandToolController)) {
		LeftHandToolController->SetHandVisibility(IsVisible);
	}
}

void AUVRPCharacter::SetControllersType(bool IsSkeletalMesh)
{
	Mesh1P->SetVisibility(IsSkeletalMesh);

	if (IsValid(RightHandToolController))
		RightHandToolController->SetHandVisibility(!IsSkeletalMesh);
	if (IsValid(LeftHandToolController))
		LeftHandToolController->SetHandVisibility(!IsSkeletalMesh);

	if (IsSkeletalMesh) {
		LeftHandDisplay->AttachToComponent(Mesh1P, FAttachmentTransformRules::KeepRelativeTransform, TEXT("DisplayLeft_Menu"));
	}
	else if (IsValid(LeftHandToolController)){
		LeftHandDisplay->AttachToComponent(LeftHandToolController->HandStaticMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("DisplayLeft_Menu"));
	}
	OnSetControllersType(IsSkeletalMesh);
}

void AUVRPCharacter::OnStartTeleport(ATeleportBase* TargetPlatform)
{
	if (CanShowTeleportPortal && IsValid(PortalCaptureTarget) && IsValid(PortalCaptureCamera) && IsValid(CurrentLocationBase)) {
		FVector pos = FVector(0.0f, 0.0f, (FirstPersonCameraComponent->GetComponentLocation().Z - CurrentLocationBase->GetActorLocation().Z) * 0.65f);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "OnStartTeleport: pos.z = " + FString::SanitizeFloat(pos.Z));

		TargetPortalPlatform = TargetPlatform;
		PortalCaptureCamera->SetActorLocation(TargetPortalPlatform->GetActorLocation() + pos);
		PortalCaptureTarget->SetActorLocation(CurrentLocationBase->GetActorLocation() + pos);
		//DrawDebugBox(GetWorld(), CurrentLocationBase->GetActorLocation() + pos, FVector(10.0f, 10.0f, 10.0f), FColor::Red, false, 5.0f);

		PortalCaptureCamera->SetActorTickEnabled(true);
		PortalCaptureTarget->SetActorHiddenInGame(false);

		IsPortalVisible = true;
	}
	/*
	if (!IsValid(PortalCaptureTarget))
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "PortalCaptureTarget is not valid");
	if (!IsValid(PortalCaptureCamera))
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "PortalCaptureCamera is not valid");
	if (!IsValid(CurrentLocationBase))
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "CurrentLocationBase is not valid");
	*/
}

void AUVRPCharacter::OnChangeTeleportPlatform(ATeleportBase* NewTargetPlatform)
{
	if (!IsPortalVisible) return;

	FVector pos = FVector(0.0f, 0.0f, FirstPersonCameraComponent->GetComponentLocation().Z - CurrentLocationBase->GetActorLocation().Z);
	TargetPortalPlatform = NewTargetPlatform;
	PortalCaptureCamera->SetActorLocation(TargetPortalPlatform->GetActorLocation() + pos);
}

void AUVRPCharacter::OnEndTeleportPlatform()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "OnEndTeleportPlatform");
	if (!IsPortalVisible) return;

	PortalCaptureCamera->SetActorTickEnabled(false);
	PortalCaptureTarget->SetActorHiddenInGame(true);

	IsPortalVisible = false;
}

void AUVRPCharacter::UpdateHandsView(bool HandsView)
{
	Mesh1P->UpdateHandsMeshType(HandsView);
}

void AUVRPCharacter::StartBackgroundMusic(FBackgroundAudioParams MusicParams)
{
	// check reenter
	if (BGMusicParams.VolumeName == MusicParams.VolumeName) return;
	StopBackgroundMusicHardly();

	BGMusicParams = MusicParams;
	float interval = MusicParams.MusicFile->GetDuration() + MusicParams.TimeoutInterval;

	GetWorldTimerManager().SetTimer(BGMusicTimer, this, &AUVRPCharacter::Timer_BackgroundMusic, interval, true, 5.0f);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "StartBackgroundMusic: " + BGMusicParams.VolumeName.ToString());
	//Timer_BackgroundMusic();
}

void AUVRPCharacter::StopBackgroundMusic(FBackgroundAudioParams MusicParams)
{
	if (IsValid(BGAudioComp) && BGMusicParams.VolumeName == MusicParams.VolumeName) {
		BGMusicParams.LoopsCount = 0;
		//Timer_BackgroundMusic();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "StopBackgroundMusic: " + MusicParams.VolumeName.ToString());
	}
}

void AUVRPCharacter::StopBackgroundMusicHardly()
{
	// hard stop if already playing - for setplatform(...) command
	if (IsValid(BGAudioComp)) {
		BGMusicParams.LoopsCount = 0;
		GetWorldTimerManager().ClearTimer(BGMusicTimer);
		BGAudioComp->Stop();
		if (IsValid(BGAudioComp)) BGAudioComp->DestroyComponent();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "StopBackgroundMusicHardly: " + BGMusicParams.VolumeName.ToString());
	}
}

void AUVRPCharacter::AllowThrowMainDataDevice()
{
	for (int i = 0; i < ControllerDataDevices.Num(); i++) {
		if (!ControllerDataDevices[i]->CanBeThrownOut) {
			ControllerDataDevices[i]->CanBeThrownOut = true;
			ControllerDataDevices[i]->IsAvalibleForTake = true;
			break;
		}
	}
}

void AUVRPCharacter::SetupGameplayTime(bool StartOrPause)
{
	if (IsValid(ActiveGameMode)) {
		if (StartOrPause)
			GameplayStatTime = GetWorld()->RealTimeSeconds;
		else {
			float fTime = GetWorld()->RealTimeSeconds;
			ActiveGameMode->GameStat.InGameTime += (fTime - GameplayStatTime);
			GameplayStatTime = fTime;
		}
	}
}

void AUVRPCharacter::PlayFeedback(float Intensity, float Duration, bool LeftLarge, bool LeftSmall, bool RightLarge, bool RightSmall, bool IsStartOrStop)
{
	EDynamicForceFeedbackAction::Type action = IsStartOrStop ? EDynamicForceFeedbackAction::Type::Start : EDynamicForceFeedbackAction::Type::Stop;
	PlayFeedbackByAction(Intensity, Duration, LeftLarge, LeftSmall, RightLarge, RightSmall, action);
}

void AUVRPCharacter::PlayFeedbackByAction_Implementation(float Intensity, float Duration, bool LeftLarge, bool LeftSmall, bool RightLarge, bool RightSmall, EDynamicForceFeedbackAction::Type Action)
{
	if (!IsValid(MyController)) {
		MyController = Cast<APlayerController>(GetController());
	}

	if (IsValid(MyController)) {
		FLatentActionInfo p;
		MyController->PlayDynamicForceFeedback(Intensity, Duration, LeftLarge, LeftSmall, RightLarge, RightSmall, Action, p);
	}
}

void AUVRPCharacter::PlaySingleFeedback(EFeedbackAction Action, bool IsRightHand)
{
	FHandFeedbackSetup inf = ActiveGameMode->GetControllerFeedbackOptions(Action);
	bool LeftLarge = false, LeftSmall = false, RightLarge = false, RightSmall = false;

	if (inf.IsLargeServo && IsRightHand)
		RightLarge = true;
	else if (inf.IsLargeServo && !IsRightHand)
		LeftLarge = true;
	else if (!inf.IsLargeServo && IsRightHand)
		RightSmall = true;
	else if (!inf.IsLargeServo && !IsRightHand)
		LeftSmall = true;

	PlayFeedbackByAction(inf.DefaultIntensity, inf.DefaultDuration, LeftLarge, LeftSmall, RightLarge, RightSmall, EDynamicForceFeedbackAction::Start);
}

void AUVRPCharacter::UpdateFeedbackByAction(FHandFeedbackSetup Options, bool IsRightHand, EDynamicForceFeedbackAction::Type FeedBackAction)
{
	FHandFeedbackSetup inf = Options;
	bool LeftLarge = false, LeftSmall = false, RightLarge = false, RightSmall = false;

	if (inf.IsLargeServo && IsRightHand)
		RightLarge = true;
	else if (inf.IsLargeServo && !IsRightHand)
		LeftLarge = true;
	else if (!inf.IsLargeServo && IsRightHand)
		RightSmall = true;
	else if (!inf.IsLargeServo && !IsRightHand)
		LeftSmall = true;

	PlayFeedbackByAction(inf.DefaultIntensity, inf.DefaultDuration, LeftLarge, LeftSmall, RightLarge, RightSmall, FeedBackAction);
}

bool AUVRPCharacter::HasDataDevice(int32 UniqueNumber)
{
	bool bRet = false;
	
	for (int i = 0; i < ControllerDataDevices.Num(); i++) {
		if (ControllerDataDevices[i]->UniqueNumber == UniqueNumber) {
			bRet = true; break;
		}
	}

	return bRet;
}

void AUVRPCharacter::OnDataDeviceSelected(int32 UniqueNumber)
{
	// no data module descritions when loading game
	if (IsValid(ActiveGameMode) && ActiveGameMode->IsLoadingGame) return;

	if (IsValid(VoicePreset)) {
		int dm_active = 0;
		for (int i = 0; i < ControllerDataDevices.Num(); i++) {
			if (ControllerDataDevices[i]->UniqueNumber == UniqueNumber) { dm_active = i; break; }
		}

		switch (UniqueNumber) {
			case 1:
				VoicePreset->PlayElementByQuote(EVoiceQuote::VO_C_Gl_MissionDataModule, true);
				break;
			case 2:
				if (ControllerDataDevices[dm_active]->CanBeDecrypted()) {
					VoicePreset->PlayElementByQuote(EVoiceQuote::VO_C_Gl_EnoughCodes);
				}
				else {
					VoicePreset->PlayElementByQuote(EVoiceQuote::VO_C_Gl_NotEnoughCodes);
				}
				break;
			case 3:
				if (!HasDataDevice(4 /*ControllerDataDevices[dm_active]->RequiredDataModule*/)) {
					VoicePreset->PlayElementByQuote(EVoiceQuote::VO_C_Gl_ModuleDecrypted, true);
				}
				break;
		}
	}
}

void AUVRPCharacter::ShowHelpMessage(EHelpMessage MessageType)
{
	if (!IsValid(ActiveGameMode)) return;
	if (!ActiveGameMode->GO_GameplayTutorial) return;
	if (ActiveGameMode->IsLoadingGame) return;

	if (IsValid(LastMessageWindow)) {
		LastMessageWindow->Destroy();
		LastMessageWindow = nullptr;
	}

	// to avoid a repeated ьуыыфпуы
	if (HelpMessagesUsed.Find(MessageType) != INDEX_NONE) return;

	HelpMessagesUsed.Add(MessageType);

	FString msg = "";
	switch (MessageType) {
		case EHelpMessage::M_DataModuleTaken:
			msg = "It's you first data module. Use trigger to enable it\nand grip button to put it on the floor.";
			break;
		case EHelpMessage::M_DataModuleDroppedDisabled:
			if (HelpMessagesUsed.Find(EHelpMessage::M_DataModuleDroppedEnabled) != INDEX_NONE) {
				msg = "Touch module or use tool beam to pick it up.";
			}
			else {
				msg = "Try to drop enabled module next time.\nTouch module or use tool beam to pick it up.";
			}
			break;
		case EHelpMessage::M_DataModuleDroppedEnabled:
			msg = "Use tool beam to read or hand tools\nto decrypt module if it's possible.\nTouch module or use tool beam to pick it up.";
			break;
	}

	LastMessageWindow = ActiveGameMode->ShowMessage(FText::FromString(msg), 15.0f);
}

void AUVRPCharacter::Timer_BackgroundMusic()
{
	if (BGMusicParams.LoopsCount > 0 && !IsInMainMenu()) {
		BGMusicParams.LoopsCount--;
		
		if (!IsValid(BGAudioComp)) {
			float vol = 1.0f;
			if (IsValid(ActiveGameMode)) vol = 1.0f; //ActiveGameMode->GO_MusicVolume;
			BGAudioComp = UGameplayStatics::CreateSound2D(GetWorld(), BGMusicParams.MusicFile, vol); // :PlaySound2D(GetWorld(), BGMusicParams.MusicFile);
		}
		BGAudioComp->Play(0.0f);
		BGAudioPlayingStartTime = GetWorld()->GetRealTimeSeconds();
	}
	else if (BGMusicParams.LoopsCount == 0) {
		GetWorldTimerManager().ClearTimer(BGMusicTimer);
		if (IsValid(BGAudioComp)) BGAudioComp->DestroyComponent();
	}
}

// Calculate Offset Relative to Playing Room Borders (to make sure whole level is inside the chaperone)
FVector AUVRPCharacter::CalculateMainLevelOffest()
{
	FVector vRet = FVector::ZeroVector;
	const float SceneWidth = 3.0f;
	
	/*
	TArray<FVector> bnds = SteamVRChaperone->GetBounds();
	float MinX = 9999.0f, MaxX = -9999.0f;
	float PlayingAreaWidth;
	for (int i = 0; i < bnds.Num(); i++) {
		if (bnds[i].X < MinX) MinX = bnds[i].X;
		if (bnds[i].X > MaxX) MaxX = bnds[i].X;
	}
	if (MinX == 9999.0f || MaxX == -9999.0f) return vRet;
	PlayingAreaWidth = MaxX - MinX;

	if (PlayingAreaWidth < SceneWidth)
		vRet.X = 1.0f;
	else
		vRet.X = 0.5f;
	*/
	//vRet.X = (PlayingAreaWidth - SceneWidth) / 2.0f;
	
	return FVector::ZeroVector; // vRet;
}
