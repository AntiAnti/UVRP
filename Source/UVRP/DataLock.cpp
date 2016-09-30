// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "DataDevice.h"
#include "HandController.h"
#include "Engine.h"
#include "MenuInteractionInterface.h"
#include "UVRPCharacter.h"
#include "DataLock.h"

ADataLock::ADataLock()
{
	this->PrimaryActorTick.bStartWithTickEnabled = false;
	this->PrimaryActorTick.bCanEverTick = true;

	////////// MAIN MESH //////////

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smDataMesh(TEXT("StaticMesh'/Game/FirstPerson/Meshes/data_lock_2.data_lock_2'"));
	if (smDataMesh.Object != NULL) {
		GetStaticMeshComponent()->SetStaticMesh(smDataMesh.Object);
	}
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	WiresMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WiresMesh"));
	WiresMesh->SetupAttachment(GetStaticMeshComponent());
	WiresMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.4f));
	WiresMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smWiresMesh(TEXT("StaticMesh'/Game/FirstPerson/Meshes/data_lock_wires.data_lock_wires'"));
	if (smWiresMesh.Object != NULL) {
		WiresMesh->SetStaticMesh(smWiresMesh.Object);
	}
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	//this->Tags.Add
	
	////////// COMPONENTS //////////
	/*
	ExternalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ExternalTrigger"));
	ExternalTrigger->AttachParent = GetStaticMeshComponent();
	ExternalTrigger->SetRelativeLocation(FVector::ZeroVector);
	ExternalTrigger->SetBoxExtent(FVector(90.0f, 90.0f, 90.0f));
	ExternalTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExternalTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ExternalTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ExternalTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	ExternalTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADataLock::External_OnBeginOverlap);
	ExternalTrigger->OnComponentEndOverlap.AddDynamic(this, &ADataLock::External_OnEndOverlap);
	*/
	PositionalTrigger1 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger1"));
	PositionalTrigger1->SetupAttachment(GetStaticMeshComponent());
	PositionalTrigger1->SetRelativeLocation(FVector(-4.13f, 0.0f, 0.8f));
	PositionalTrigger1->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger1->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PositionalTrigger1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger1->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger1->bHiddenInGame = true;
	PositionalTrigger1->ComponentTags.Add(TAG_DataLock);

	PositionalTrigger2 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger2"));
	PositionalTrigger2->SetupAttachment(GetStaticMeshComponent());
	PositionalTrigger2->SetRelativeLocation(FVector(2.67f, 3.2f, 0.8f));
	PositionalTrigger2->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger2->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PositionalTrigger2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger2->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger2->bHiddenInGame = true;
	PositionalTrigger2->ComponentTags.Add(TAG_DataLock);

	PositionalTrigger3 = CreateDefaultSubobject<UBoxComponent>(TEXT("PositionalTrigger3"));
	PositionalTrigger3->SetupAttachment(GetStaticMeshComponent());
	PositionalTrigger3->SetRelativeLocation(FVector(2.67f, -3.2f, 0.8f));
	PositionalTrigger3->SetBoxExtent(FVector(1.5f, 1.5f, 1.5f));
	PositionalTrigger3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PositionalTrigger3->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PositionalTrigger3->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PositionalTrigger3->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PositionalTrigger3->bHiddenInGame = true;
	PositionalTrigger3->ComponentTags.Add(TAG_DataLock);

	TurnsPerSecond = 1.0f;
	CurrentCircle = 0.0f;
	IsLockEnabledOnStart = true;
	IsCanAcceptDataDevice = true;
	//IsWiresMeshVisible = true;

	SetLight(IsLockEnabledOnStart);
	WiresMesh->SetVisibility(IsWiresMeshVisible);
}

void ADataLock::BeginPlay()
{
	Super::BeginPlay();

	mti_LightLockL0 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(1);
	mti_LightLockL1 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(3);
	mti_LightLockL2 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(5);
	mti_WiresLockL0 = WiresMesh->CreateDynamicMaterialInstance(1);
	mti_WiresLockL1 = WiresMesh->CreateDynamicMaterialInstance(3);

	IsCanAcceptDataDevice = true;
	WiresMesh->SetVisibility(IsWiresMeshVisible);
	SetLight(IsLockEnabledOnStart);
	this->SetActorTickEnabled(false);

	if (IsValid(ChildDataModule)) {
		ChildDataModule->AttachToController(this);
	}
}

/*
void ADataLock::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	WiresMesh->SetVisibility(IsWiresMeshVisible);
	SetLight(IsLockEnabledOnStart);
}
*/

void ADataLock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentCircle += TurnsPerSecond * DeltaTime;
	if (IsValid(mti_LightLockL0))
		mti_LightLockL0->SetScalarParameterValue("Coord", CurrentCircle);
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "mti_LightLockL0 is not valid");
}

void ADataLock::SetLight(bool Value)
{
	const float fEmissionPower = 1.3f;
	if (!IsValid(mti_LightLockL0)) mti_LightLockL0 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(1);
	if (!IsValid(mti_LightLockL1)) mti_LightLockL1 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(3);
	if (!IsValid(mti_LightLockL2)) mti_LightLockL2 = GetStaticMeshComponent()->CreateDynamicMaterialInstance(5);

	if (mti_LightLockL0 != nullptr) mti_LightLockL0->SetScalarParameterValue("EmissivePower", Value ? fEmissionPower : 0.0f);
	if (mti_LightLockL1 != nullptr) mti_LightLockL1->SetScalarParameterValue("EmissivePower", Value ? fEmissionPower : 0.0f);
	if (mti_LightLockL2 != nullptr) mti_LightLockL2->SetScalarParameterValue("EmissivePower", Value ? fEmissionPower : 0.0f);

	SetWiresLight(Value);
}

void ADataLock::SetWiresLight(bool Value)
{
	const float fEmissionPower = 1.3f;
	if (!IsValid(mti_WiresLockL0)) mti_WiresLockL0 = WiresMesh->CreateDynamicMaterialInstance(1);
	if (!IsValid(mti_WiresLockL1)) mti_WiresLockL1 = WiresMesh->CreateDynamicMaterialInstance(3);

	if (mti_WiresLockL0 != nullptr) mti_WiresLockL0->SetScalarParameterValue("EmissivePower", Value ? fEmissionPower : 0.0f);
	if (mti_WiresLockL1 != nullptr) mti_WiresLockL1->SetScalarParameterValue("EmissivePower", Value ? fEmissionPower : 0.0f);
}

void ADataLock::SetControllerConnection(bool Value, AHandController *controller)
{
	ControllerConnected = Value;

	CurrentCircle = 0.0f;
	if (!ControllerConnected) {
		if (mti_LightLockL0)
			mti_LightLockL0->SetScalarParameterValue("Coord", CurrentCircle);
	} 
	this->SetActorTickEnabled(Value);
	if (Value)
		ActiveController = controller;
	else
		ActiveController = nullptr;
}

void ADataLock::OnDataDeviceAttached(bool IsAttachedOrDetached)
{
	if (!IsValid(LockOwnerActor) && IsValid(GetRootComponent()->GetAttachParent())) {
		AActor* owner = GetRootComponent()->GetAttachParent()->GetOwner();

		IMenuInteractionInterface *test = Cast<IMenuInteractionInterface>(owner);
		if (test != nullptr) {
			LockOwnerActor = owner;
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "running OnDataDeviceAttached()");
	if (IsWiresMeshVisible) {
		SetWiresLight(IsAttachedOrDetached);
		//SetLight(IsAttachedOrDetached);
	}

	if (IsValid(LockOwnerActor)) {
		IMenuInteractionInterface *recipient = Cast<IMenuInteractionInterface>(LockOwnerActor);
		if (recipient != nullptr) {
			if (IsAttachedOrDetached) {
				recipient->OnClick(FVector(1.0f, 1.0f, 1.0f));
			}
			else {
				recipient->OnClick(FVector::ZeroVector);
			}
		}
	}
}

bool ADataLock::OnClick(FVector HitPoint)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "ADataLock::OnClick");
	bool bRet = false;
	if (IsActorTickEnabled() && ActiveController != nullptr) {
		float val = FMath::Frac(CurrentCircle);
		if (val >= 0.9f || val <= 0.1f) {
			if (IsValid(ChildDataModule)) {
				IsCanAcceptDataDevice = false;
				
				//ChildDataModule->DetachFormController();
				ChildDataModule->AttachToController(ActiveController);

				AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				if (player != nullptr) {
					player->TakeDataDevice(ChildDataModule);
					ActiveController->CurrentDataDevice = player->ControllerDataDevices.Num() - 1;

					/////////////////// FEEDBACK ///////////////////
					player->PlaySingleFeedback(EFeedbackAction::A_CatchAndDrop, ActiveController->IsRightHand());
					////////////////////////////////////////////////
				}

				ChildDataModule = nullptr;
				SetWiresLight(false);
			}
			OnDataDeviceAttached(false);

			if (hTimerReactivate.IsValid()) GetWorldTimerManager().ClearTimer(hTimerReactivate);
			GetWorldTimerManager().SetTimer(hTimerReactivate, this, &ADataLock::Timer_Reactivate, 1.0f, false, 1.0f);

			bRet = true;
		}
	}

	return bRet;
}

void ADataLock::SetWiresVisibility(bool IsVisible)
{
	IsWiresMeshVisible = IsVisible;
	WiresMesh->SetVisibility(IsWiresMeshVisible, false);
	GetStaticMeshComponent()->SetVisibility(true, false);
}

void ADataLock::Timer_Reactivate()
{
	IsCanAcceptDataDevice = true;
	GetWorldTimerManager().ClearTimer(hTimerReactivate);
	hTimerReactivate.Invalidate();
}

/*
void ADataLock::External_OnBeginOverlap(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->IsA(AHandController::StaticClass())) SetLight(true);
}

void ADataLock::External_OnEndOverlap(AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHandController::StaticClass())) SetLight(false);
}
*/