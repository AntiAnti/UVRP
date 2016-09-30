// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "MenuInteractionInterface.h"
#include "Engine.h"
#include "DataDevice.h"
#include "UVRPCharacter.h"
#include "HandController.h"
#include "DrawDebugHelpers.h"
#include "MenuCollisionBox.h"
#include "ControllerMenuPointer.h"


UControllerMenuPointer::UControllerMenuPointer()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAssetPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/controller_menu_pointer2.controller_menu_pointer2'"));
	if (smAssetPlane.Object != NULL) {
		SetStaticMesh(smAssetPlane.Object);
	}
	/*
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("PointerBoxCollider");
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->AttachParent = GetAttachParent();
	BoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//BoxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	BoxCollider->ComponentTags.Add(TEXT("ControllerToolLine"));
	*/

	CurrentMenuItemName_Actor = "";
	CurrentMenuItemName_Comp = "";
	SetLength(100.0f);
	ComponentTags.Add(TEXT("ControllerToolLine"));
}

void UControllerMenuPointer::BeginPlay()
{
	Super::BeginPlay();

	stTraceParams = FCollisionQueryParams(FName(TEXT("HandControllerTrace")), true, this->GetAttachmentRootActor());
	stTraceParams.bTraceComplex = true;
	stTraceParams.bTraceAsyncScene = true;
	stTraceParams.bReturnPhysicalMaterial = false;

	AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	stTraceParams.AddIgnoredActor(player->GetRightController());
	stTraceParams.AddIgnoredActor(player->GetLeftController());
	stTraceParams.AddIgnoredActor(player);
	stTraceParams.bTraceComplex = true;
	stTraceParams.OwnerTag = TEXT("IGNORE");
}

void UControllerMenuPointer::SetLength(float Length)
{
	fLineLength = Length;

	FVector sc;
	sc.Y = sc.Z = 1.0f;
	sc.X = fLineLength / 100.0f;
	SetRelativeScale3D(sc);

	//sc.Y = sc.X = 0.05f;
	//BoxCollider->SetWorldScale3D(sc);
}

void UControllerMenuPointer::TracePlayerSelectorLine(float DeltaTime)
{
	FString NewItemActorName = "", NewItemCompName = "";

	/////////////////////////////////////////////////////////////////////////////////////
	if (!this->IsVisible()) return;

	FHitResult stHit(ForceInit);
	FVector vStart = GetComponentLocation();
	FVector vEnd;
	vEnd = vStart + GetComponentRotation().Vector() * 100.0f; // fLineLength;
	vStart += GetComponentRotation().Vector() * 4.0f;

	//call GetWorld() from within an actor extending class
	FCollisionResponseParams d;
	d.CollisionResponse.SetAllChannels(ECollisionResponse::ECR_Ignore);
	d.CollisionResponse.SetResponse(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	d.CollisionResponse.SetResponse(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	d.CollisionResponse.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	d.CollisionResponse.SetResponse(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetWorld()->LineTraceSingleByChannel(stHit, vStart, vEnd, /*ECC_ControllerResponses*/ ECollisionChannel::ECC_Visibility, stTraceParams, d);

	//DrawDebugLine(GetWorld(), vStart, vEnd, FColor::White, false, -1, 0, 1.0f);
	//DrawDebugLine(GetWorld(), vStart, vStart * 2 - vEnd, FColor::Red, false, -1, 0, 1.0f);

	float dist;
	if (stHit.bBlockingHit && stHit.IsValidBlockingHit()) {
		dist = stHit.Distance + 4.0f;
		/*
		for (int i = 0; i < 10; i++)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, stHit.GetActor()->GetName());
		*/
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, stHit.GetActor()->GetName() + " - " + stHit.GetComponent()->GetName());
		
		if (stHit.Actor->IsA(AHandController::StaticClass())) {
			AUVRPCharacter *player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
			if (IsValid(player)) {
				stTraceParams.AddIgnoredActor(player->GetRightController());
				stTraceParams.AddIgnoredActor(player->GetLeftController());
			}
			SetLength(100.0f);
		}
		else {
			TraceBlockingPoint = stHit.Location;
			if (dist < 5.0f)
				SetLength(5.0f);
			else
				SetLength(dist);
			
			if (!stHit.Actor.Get()->IsA(AUVRPCharacter::StaticClass())) {
				IMenuInteractionInterface *p = Cast<IMenuInteractionInterface>(stHit.GetActor());
				if (stHit.GetActor()->IsA(AMenuCollisionBox::StaticClass()) || p != nullptr)
				{
					CurrentEventRecipient = stHit.GetActor();
				}
				else if (stHit.GetActor()->IsA(ADataDevice::StaticClass())) {
					ADataDevice* dm = Cast<ADataDevice>(stHit.GetActor());

					if (IsValid(dm) && dm->IsVisible()) {
						CurrentEventRecipient = stHit.GetActor();
					}
				}
				/*
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, stHit.GetActor()->GetName() + " doesn't implement IMenuInteractionInterface");
				}
				*/
			}
		}

		/*
		if (stHit.GetActor()->ActorHasTag("ControllerToolLine")) {
		AHandController *hand = Cast<AHandController>(GetOwner());
		if (hand) {
		CurrentEventRecipient = stHit.GetActor();
		}
		}
		*/
	}
	else {
		SetLength(100.0f);
		CurrentEventRecipient = nullptr;
	}
}

void UControllerMenuPointer::AddIgnoredComponent(UPrimitiveComponent* Component)
{
	stTraceParams.AddIgnoredComponent(Component);
}

void UControllerMenuPointer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//TracePlayerSelectorLine(DeltaTime);
}