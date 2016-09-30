// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "HandController.h"
#include "DataLock.h"
#include "UVRPCharacter.h"
#include "UVRPGameMode.h"
#include "DataModuleViewList.h"
#include "DataJigsaw.h"
#include "DataDevice.h"

#define SOCKET_DataDevice "Socket_DataDevice"

ADataDevice::ADataDevice()
{
	DisplayOffsetHand = 80.0f;
	DisplayOffsetFloor = 100.0f;

	// StaticMeshComponent /////////////////////
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smDataMesh(TEXT("StaticMesh'/Game/FirstPerson/Meshes/data_device.data_device'"));
	if (smDataMesh.Object != NULL) {
		GetStaticMeshComponent()->SetStaticMesh(smDataMesh.Object);
	}
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->bCastStaticShadow = false;
	GetStaticMeshComponent()->bCastDynamicShadow = false;
	//GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, 0.1f);
	//GetStaticMeshComponent()->SetSimulatePhysics(false);

	// ProjectorEmitter /////////////////////
	ProjectorEmitter = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectorEmitter"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> emLighting(TEXT("ParticleSystem'/Game/FirstPerson/Effects/Effects/FX_Ambient/Lighting/P_LightCone_LOD.P_LightCone_LOD'"));
	if (emLighting.Object != NULL) {
		ProjectorEmitter->SetTemplate(emLighting.Object);
	}
	ProjectorEmitter->SetupAttachment(GetStaticMeshComponent());
	ProjectorEmitter->bAutoActivate = false;
	ProjectorEmitter->Deactivate();
	ProjectorEmitter->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());

	// ProjectorPlane /////////////////////
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smProjectorMesh(TEXT("StaticMesh'/Game/FirstPerson/Meshes/plane100x100.plane100x100'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mtDisplay(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_DataDeviceDisplay.mti_DataDeviceDisplay'"));

	ProjectorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectorPlane"));
	ProjectorPlane->SetupAttachment(GetStaticMeshComponent());
	ProjectorPlane->SetRelativeLocation(FVector(0.0f, 0.0f, DisplayOffsetHand));
	ProjectorPlane->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	ProjectorPlane->SetVisibility(false);
	if (smProjectorMesh.Object != NULL) {
		ProjectorPlane->SetStaticMesh(smProjectorMesh.Object);
	}
	if (mtDisplay.Object != NULL) {
		ProjectorPlane->SetMaterial(0, mtDisplay.Object);
	}

	// ControllerCollider /////////////////////
	ControllerCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ControllerCollider"));
	ControllerCollider->SetupAttachment(GetStaticMeshComponent());
	ControllerCollider->SetBoxExtent(FVector(1.0f, 1.0f, 0.5f));
	ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ControllerCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//ControllerCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ControllerCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ControllerCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ControllerCollider->bGenerateOverlapEvents = true;
	ControllerCollider->OnComponentBeginOverlap.AddDynamic(this, &ADataDevice::MainTrigger_OnBeginOverlap);

	IsAvalibleForTake = true;
	IsAvalibleForUnlock = false;
	IsEncrypted = false;
	IsShowingData = false;
	CanBeThrownOut = false;
	IsDisplayActive = true;
	ItemColor = FColor::Black;

	if (ParentLockActor != nullptr) AttachToController(ParentLockActor);
}

void ADataDevice::BeginPlay()
{
	Super::BeginPlay();
	mti_LightCross = GetStaticMeshComponent()->CreateDynamicMaterialInstance(0);
	mti_DisplayMain = ProjectorPlane->CreateDynamicMaterialInstance(0);

	// initialize attachment
	IsAttachedToActor = IsValid(ParentLockActor);

	if (IsAttachedToActor) {
		if (AttachToController(ParentLockActor)) {
			ADataLock *lock = Cast<ADataLock>(ParentLockActor);
			if (lock != nullptr)
				SetLight(lock->IsLockEnabledOnStart);
			else
				SetLight(true);
		}
	}

	if (IsAttachedToActor) {
		ADataLock *p = Cast<ADataLock>(ParentLockActor);
		if (p != nullptr) p->ChildDataModule = this;
	}

	// initialize materials
	if (ItemColor != FColor::Black && mti_LightCross != nullptr) {
		mti_LightCross->SetVectorParameterValue("ColorMultiplier", ItemColor);
	}

	// initialize collision
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	ControllerCollider->SetRelativeLocation(FVector::ZeroVector);
	ControllerCollider->SetBoxExtent(FVector(2.0f, 2.0f, 1.0f));
	ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ControllerCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ControllerCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ControllerCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ControllerCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ControllerCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ControllerCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ControllerCollider->bHiddenInGame = true;
	ControllerCollider->bGenerateOverlapEvents = true;

	// init puzzle for module unlocking
	if (IsEncrypted && IsAvalibleForUnlock) {
		ADataJigsaw::ResetJigsaw(this);
	}

	IsDisplayActive = true;

	PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

bool ADataDevice::AttachToController(AActor *NewParent)
{
	if (NewParent == nullptr) return false;
	ControllerCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	// for roller
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	UStaticMeshComponent *p = nullptr;
	AHandController *ctrl = Cast<AHandController>(NewParent);
	bool bRet = false;
	if (ctrl != nullptr) {
		p = ctrl->ToolMesh;
		SetLight(false);
	}
	else {
		AStaticMeshActor *sm = Cast<AStaticMeshActor>(NewParent);
		if (sm != nullptr) {
			p = sm->GetStaticMeshComponent();
			SetLight(true);
		}
	}

	ControllerCollider->SetBoxExtent(FVector(1.0f, 1.0f, 0.5f));
	if (p != nullptr) {
		GetStaticMeshComponent()->SetSimulatePhysics(false);
		this->AttachToActor(NewParent, FAttachmentTransformRules::KeepWorldTransform, SOCKET_DataDevice);
		this->AttachToComponent(p, FAttachmentTransformRules::KeepWorldTransform, SOCKET_DataDevice);
		//this->AttachRootComponentTo(p, SOCKET_DataDevice, EAttachLocation::Type::KeepWorldPosition);
		//this->SetActorRelativeLocation(FVector::ZeroVector);

		IsAttachedToActor = true;
		ControllerCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		this->SetActorLocation(p->GetSocketLocation(SOCKET_DataDevice));
		this->SetActorRotation(p->GetSocketQuaternion(SOCKET_DataDevice));

		ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		if (IsValid(ctrl) && IsValid(PlayerOwner)) {
			if (UniqueNumber != 1) {
				PlayerOwner->OnDataDeviceSelected(UniqueNumber);

				PlayerOwner->ShowHelpMessage(EHelpMessage::M_DataModuleTaken);
			}
		}
		SetVisibility(true);

		bRet = true;
	}

	return bRet;
}

void ADataDevice::DetachFormController()
{
	if (!IsShowingData) {
		//this->DetachRootComponentFromParent(true);
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		IsAttachedToActor = false;
		SetLight(false);
	}
}

void ADataDevice::SetLight(bool Value)
{
	if (mti_LightCross != nullptr) {
		mti_LightCross->SetScalarParameterValue("EmissivePower", Value ? 1.0f : 0.0f);
	}
}

void ADataDevice::ThrowDevice(FVector AngularVelocity, FVector LinearVelocity)
{
	DetachFormController();

	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetEnableGravity(true);
	GetStaticMeshComponent()->SetPhysicsAngularVelocity(AngularVelocity * 1.5f);
	GetStaticMeshComponent()->SetPhysicsLinearVelocity(LinearVelocity * 1.5f);
	PrevLocation = GetStaticMeshComponent()->GetComponentLocation();

	bool tmp = IsShowingData;
	SetProjectorEnabled(false);
	IsShowingData = tmp;

	GetWorldTimerManager().SetTimer(hThrowTimer, this, &ADataDevice::ThrowTimer_Tick, 1.0f, true, 1.0f);
}

void ADataDevice::SetVisibility(bool IsVisible)
{
	GetStaticMeshComponent()->SetVisibility(IsVisible);

	if (IsVisible) {
		ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else {
		ControllerCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool ADataDevice::IsVisible()
{
	return GetStaticMeshComponent()->IsVisible();
}

void ADataDevice::SetProjectorEnabled(bool Value)
{
	if (Value && !IsDisplayActive) return;

	IsShowingData = Value;
	SetLight(Value);
	if (Value) ProjectorEmitter->Activate();
	else ProjectorEmitter->Deactivate();

	// make sure we'll be able to pick it up
	if (Value) GetStaticMeshComponent()->SetSimulatePhysics(false);

	if (!IsValid(PlayerOwner))
		PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// show jigsaw
	if (IsEncrypted && IsAvalibleForUnlock && !IsAttachedToActor) {
		ProjectorPlane->SetVisibility(false);

		if (IsValid(CurrentJigsaw)) {
			CurrentJigsaw->SaveJigsaw();
			CurrentJigsaw->Destroy();
			CurrentJigsaw = nullptr;
		}

		if (Value) {
			FActorSpawnParameters params;
			
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			CurrentJigsaw = GetWorld()->SpawnActor<ADataJigsaw>(ADataJigsaw::StaticClass(), FTransform(FVector(0.0f, 0.0f, DisplayOffsetFloor)), params);
			CurrentJigsaw->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
			CurrentJigsaw->InitParentDataDevice(this);
			CurrentJigsaw->LoadJigsaw();
		}
	}
	// show plane message
	else if (IsEncrypted || (RequiredDataModule > 0 && IsValid(PlayerOwner) && !PlayerOwner->HasDataDevice(RequiredDataModule))) {
		// show unlocking puzzle or error message
		if (mti_DisplayMain != nullptr) {
			if (Value) {
				if (this->GetAttachParentActor() != nullptr) {
					// attached to controller
					ProjectorPlane->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
					ProjectorPlane->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
					ProjectorPlane->SetRelativeLocation(FVector(0.0f, 0.0f, DisplayOffsetHand));
				}
				else {
					// on the floor
					ProjectorPlane->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
					FRotator r = GetStaticMeshComponent()->GetComponentRotation();
					r.Roll = 0.0f; r.Pitch = 0.0f;
					GetStaticMeshComponent()->SetWorldRotation(r);
					ProjectorPlane->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f).Quaternion());
					ProjectorPlane->SetRelativeLocation(FVector(0.0f, 0.0f, DisplayOffsetFloor));
				}
			}
			if (ProjectorPlane) ProjectorPlane->SetVisibility(Value);

			// Show Message to explain device encryption status
			float DispNum;

			if (RequiredDataModule > 0) {
				DispNum = 3.0f;									// needs a guest code
			}
			else {
				DispNum = IsAvalibleForUnlock ? 2.0f : 1.0f;	// needs jigsaw or can't be unlocked
			}
			mti_DisplayMain->SetScalarParameterValue("DisplayRGBNumber", DispNum);
		}
	}
	// show explorer
	else {
		// show or hide data module viewer
		if (ActiveListView != nullptr && IsValid(ActiveListView)) ActiveListView->DestroyList();
		if (Value) {
			FTransform spT;
			spT.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			if (this->GetAttachParentActor() != nullptr) {
				// attached to controller
				spT.SetLocation(FVector(0.0f, 0.0f, DisplayOffsetHand));
				spT.SetRotation(FRotator(-90.0f, -180.0f, 0.0f).Quaternion());
			}
			else {
				// on the floor
				spT.SetLocation(FVector(0.0f, 0.0f, DisplayOffsetFloor));
				spT.SetRotation(FRotator(0.0f, -180.0f, 0.0f).Quaternion());
				/*
				if (GetActorRotation().Vector().Z > 0.0f) {
					spT.SetLocation(FVector(0.0f, 0.0f, DisplayOffsetFloor));
					spT.SetRotation(FRotator(0.0f, -180.0f, 0.0f).Quaternion());
				} else {
					spT.SetLocation(FVector(0.0f, 0.0f, DisplayOffsetFloor));
					spT.SetRotation(FRotator(0.0f, 180.0f, 0.0f).Quaternion());
				}
				*/
			}
			FActorSpawnParameters params;

			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ActiveListView = GetWorld()->SpawnActor<ADataModuleViewList>(ADataModuleViewList::StaticClass(), spT, params);
			ActiveListView->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, /* ToActor(this,*/ NAME_None);
			ActiveListView->DataDeviceOwner = this;

			ActiveListView->ListData.Empty();
			for (int i = 0; i < ModuleDataTree.Num(); i++)
				ActiveListView->ListData.Add(ModuleDataTree[i]);

			ActiveListView->Navigate(0);
		}
		else ActiveListView = nullptr;
	}
}

void ADataDevice::DisableCollision(USceneComponent* NewParentComponent)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "DisableCollision");
	GetStaticMeshComponent()->SetEnableGravity(false);
	GetStaticMeshComponent()->SetSimulatePhysics(false);
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	//GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (IsValid(NewParentComponent)) {
		this->AttachToComponent(NewParentComponent, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		SetActorRotation(FRotator::ZeroRotator);
	}

	ControllerCollider->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	//ControllerCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

bool ADataDevice::GetJigsawPartAvalibility(uint8 Index)
{
	if (Index < JigsawData.Num())
		return JigsawData[Index].IsVisible;
	else
		return false;
}

uint8 ADataDevice::GetJigsawState(uint8 Index)
{
	if (Index < JigsawData.Num())
		return JigsawData[Index].RotationState;
	else
		return 0;
}

void ADataDevice::SetJigsawPartAvalibility(uint8 Index, bool NewAvalibility)
{
	if (Index < JigsawData.Num())
		JigsawData[Index].IsVisible = NewAvalibility;
}

void ADataDevice::SetJigsawState(uint8 Index, uint8 NewState)
{
	if (Index < JigsawData.Num())
		JigsawData[Index].RotationState = NewState;
}

void ADataDevice::MainTrigger_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	/*
	AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(gm) && OtherActor->IsA(AHandController::StaticClass())) {
		FString txt;
		if (!IsAttachedToActor) txt = "IsAttachedToActor = false";
		if (GetStaticMeshComponent()->IsSimulatingPhysics()) txt += "\nIsSimulatingPhysics = true";

		gm->ShowMessage(FText::FromString(txt));
	}
	*/

	if (!IsAttachedToActor && OtherActor->IsA(AHandController::StaticClass()) && !GetStaticMeshComponent()->IsSimulatingPhysics() && !OtherComp->ComponentHasTag(TEXT("ControllerToolLine"))) {
		//Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->ShowMessage(FText::FromString("MainTrigger_OnBeginOverlap: " + OtherComp->GetName()), 20.0f);

		AHandController *ctrl = Cast<AHandController>(OtherActor);
		if (IsValid(ctrl) && ctrl->AcceptDataDeviceAttaching()) {
			SetProjectorEnabled(false);

			AttachToController(OtherActor);

			if (!IsValid(PlayerOwner))
				PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

			if (IsValid(PlayerOwner)) {
				PlayerOwner->TakeDataDevice(this);
				ctrl->CurrentDataDevice = PlayerOwner->ControllerDataDevices.Num() - 1;
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "ADataDevice::MainTrigger_OnBeginOverlap - AHandController::AcceptDataDeviceAttaching() returned false");
		}
	}
}

void ADataDevice::ThrowTimer_Tick()
{
	FVector dist = GetStaticMeshComponent()->GetComponentLocation() - PrevLocation;

	if (dist.Size() < 0.1f) // 1 mm
	{
		StopFalling();
	}
	else
	{
		PrevLocation = GetStaticMeshComponent()->GetComponentLocation();
	}
}

int32 ADataDevice::UnlockRandomJigsawElement()
{
	int Index;

	TArray<int> closedpieces;
	for (int i = 0; i < JigsawData.Num(); i++) {
		if (!JigsawData[i].IsVisible)
			closedpieces.Add(i);
	}

	if (closedpieces.Num() > 0) {
		Index = closedpieces[FMath::RandRange(0, closedpieces.Num() - 1)];
		JigsawData[Index].IsVisible = true;
	}
	else {
		Index = -1;
	}

	return Index;
}

void ADataDevice::OnJigsawPieceComplete(int32 PiecesCompleted)
{
	if (IsValid(PlayerOwner)) {
		if (PiecesCompleted == 5) {
			PlayerOwner->PlayVoice(EVoiceQuote::VO_C_Gl_DecryptProcessWarning1);
		}
		else if (PiecesCompleted == 10) {
			PlayerOwner->PlayVoice(EVoiceQuote::VO_C_Gl_DecryptProcessWarning2);
		}
	}
}

void ADataDevice::OnJigsawComplete()
{
	if (IsValid(PlayerOwner)) {
		PlayerOwner->PlayVoice(EVoiceQuote::VO_C_Gl_DecryptProcessFinished);
	}

	if (!hJigsawCompleteTimer.IsValid()) {
		GetWorldTimerManager().SetTimer(hJigsawCompleteTimer, this, &ADataDevice::JigsawCompleteTimer_Tick, 3.0f, false);
	}
}

void ADataDevice::JigsawCompleteTimer_Tick()
{	
	GetWorldTimerManager().ClearTimer(hJigsawCompleteTimer);
	hJigsawCompleteTimer.Invalidate();

	SetProjectorEnabled(false);

	IsEncrypted = false;

	SetProjectorEnabled(true);

	// achiiiiivochka ///////////////////////////////////////////
	AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(gm)) {
		gm->UnlockAchievement(EGameAchievements::A_ModuleDecrypted);
	}
	/////////////////////////////////////////////////////////////
}

bool ADataDevice::CanBeDecrypted()
{
	bool IsJigsawFull = true;

	for (int i = 0; i < JigsawData.Num(); i++)
		if (!JigsawData[i].IsVisible) { IsJigsawFull = false; break; };

	return (IsJigsawFull && JigsawData.Num() == 25);
}

void ADataDevice::StopFalling()
{
	ControllerCollider->SetCollisionObjectType(ECC_ControllerResponses);
	if (hThrowTimer.IsValid()) {
		GetWorldTimerManager().ClearTimer(hThrowTimer);
		hThrowTimer.Invalidate();
	}

	if (!GetStaticMeshComponent()->IsSimulatingPhysics()) {
		ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		IsAttachedToActor = false;
		IsShowingData = false;
		return;
	}

	GetStaticMeshComponent()->SetSimulatePhysics(false);

	if (!IsValid(PlayerOwner)) {
		PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}

	FRotator r = FRotationMatrix::MakeFromX(this->GetActorLocation() - PlayerOwner->GetActorLocation()).Rotator();
	r.Roll = 0.0f; r.Pitch = 0.0f;
	SetActorRotation(r);

	IsAttachedToActor = false;
	if (IsShowingData) {
		SetProjectorEnabled(true);
		PlayerOwner->ShowHelpMessage(EHelpMessage::M_DataModuleDroppedEnabled);
	}
	else {
		PlayerOwner->ShowHelpMessage(EHelpMessage::M_DataModuleDroppedDisabled);
	}

	ControllerCollider->SetBoxExtent(FVector(8.0f, 8.0f, 16.0f));
	ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADataDevice::DeactivateDisplay()
{
	IsDisplayActive = false;
}

void ADataDevice::ActivateBeamInteraction()
{
	if (!IsValid(PlayerOwner)) {
		PlayerOwner = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}

	ControllerCollider->SetCollisionObjectType(ECC_ControllerResponses);
	ControllerCollider->SetBoxExtent(FVector(8.0f, 8.0f, 16.0f));
	ControllerCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	IsAttachedToActor = false;
}
