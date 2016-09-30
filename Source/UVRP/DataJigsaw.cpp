// Anton Antonov, 2016. All rights reserved.

#include "UVRP.h"
#include "HandController.h"
#include "DataDevice.h"
#include "Engine.h"
#include "UVRPGameMode.h"
#include "DataJigsaw.h"


// Sets default values
ADataJigsaw::ADataJigsaw()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/plane100x100h.plane100x100h'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mtBackground(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_Jigsaw_Background.mti_Jigsaw_Background'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> mtPieces(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_Jigsaw_Piece.mti_Jigsaw_Piece'"));

	BackgroundPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackgroundPlane"));
	SetRootComponent(BackgroundPlane);
	//BackgroundPlane->SetRelativeLocation(FVector::ZeroVector);
	//BackgroundPlane->SetRelativeRotation(FRotator::ZeroRotator);
	BackgroundPlane->SetRelativeScale3D(FVector(JIGSAW_ScaleMultiplier, JIGSAW_ScaleMultiplier, JIGSAW_ScaleMultiplier));
	BackgroundPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BackgroundPlane->SetVisibility(true);
	if (smPlane.Object != NULL) {
		BackgroundPlane->SetStaticMesh(smPlane.Object);
	}
	if (mtBackground.Object != NULL) {
		BackgroundPlane->SetMaterial(0, mtBackground.Object);
	}

	// jigsaw pieces
	for (int32 i = 0; i < 25; i++) {
		FString ind = TEXT("JigsawPiece") + FString::FromInt(i+1);
		FName CompName = FName(TCHAR_TO_ANSI(*ind));

		JigsawPiece[i] = CreateDefaultSubobject<UStaticMeshComponent>(CompName);
		JigsawPiece[i]->SetupAttachment(BackgroundPlane);
		JigsawPiece[i]->SetRelativeLocation(GetPieceLocation(i));
		JigsawPiece[i]->SetRelativeScale3D(FVector(0.222f, 0.222f, 0.222f));
		JigsawPiece[i]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		JigsawPiece[i]->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		JigsawPiece[i]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		JigsawPiece[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
		JigsawPiece[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		JigsawPiece[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		JigsawPiece[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		JigsawPiece[i]->bGenerateOverlapEvents = true;
		JigsawPiece[i]->ComponentTags.Add(TEXT("jigsawpiece"));
		JigsawPiece[i]->ComponentTags.Add(FName(TCHAR_TO_ANSI(*FString::FromInt(i))));

		if (smPlane.Object != NULL) {
			JigsawPiece[i]->SetStaticMesh(smPlane.Object);
		}
		if (mtPieces.Object != NULL) {
			JigsawPiece[i]->SetMaterial(0, mtPieces.Object);
		}

		JigsawPiece[i]->OnComponentEndOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece_EndOverlap);

		switch (i) {
			case 0:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece01_OnBeginOverlap); break;
			case 1:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece02_OnBeginOverlap); break;
			case 2:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece03_OnBeginOverlap); break;
			case 3:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece04_OnBeginOverlap); break;
			case 4:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece05_OnBeginOverlap); break;
			case 5:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece06_OnBeginOverlap); break;
			case 6:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece07_OnBeginOverlap); break;
			case 7:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece08_OnBeginOverlap); break;
			case 8:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece09_OnBeginOverlap); break;
			case 9:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece10_OnBeginOverlap); break;
			case 10:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece11_OnBeginOverlap); break;
			case 11:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece12_OnBeginOverlap); break;
			case 12:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece13_OnBeginOverlap); break;
			case 13:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece14_OnBeginOverlap); break;
			case 14:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece15_OnBeginOverlap); break;
			case 15:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece16_OnBeginOverlap); break;
			case 16:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece17_OnBeginOverlap); break;
			case 17:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece18_OnBeginOverlap); break;
			case 18:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece19_OnBeginOverlap); break;
			case 19:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece20_OnBeginOverlap); break;
			case 20:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece21_OnBeginOverlap); break;
			case 21:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece22_OnBeginOverlap); break;
			case 22:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece23_OnBeginOverlap); break;
			case 23:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece24_OnBeginOverlap); break;
			case 24:
				JigsawPiece[i]->OnComponentBeginOverlap.AddDynamic(this, &ADataJigsaw::JigsawPiece25_OnBeginOverlap); break;
		}
	}

	HighlightedPiece = -1;
}

void ADataJigsaw::BeginPlay()
{
	Super::BeginPlay();

	InitMaterials();
	//ResetJigsaw();
}

// off
void ADataJigsaw::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADataJigsaw::InitMaterials()
{
	float u, v;
	for (int i = 0; i < 25; i++) {
		if (IsValid(JigsawPiece[i])) {
			UMaterialInstanceDynamic* inst = JigsawPiece[i]->CreateDynamicMaterialInstance(0);
			u = (float)(i % 5);
			v = ((float)(i - u) / 5.0f);
			inst->SetScalarParameterValue(TEXT("u_offset"), u * 0.2f /*+ 0.05f*/);
			inst->SetScalarParameterValue(TEXT("v_offset"), v * 0.2f /*- 0.05f*/);

			mti_Piece.Add(inst);
		}
	}
}

/* Reset pieces location and data table in parent device */
void ADataJigsaw::ResetJigsaw(ADataDevice* Source, int32 DifficlutyLevel)
{
	FJigsawElement b;
	TArray<int32> avaliblecells;
	int32 locindex;
	int32 showmax;

	if (IsValid(Source)) {
		// how many pieces we should show st start depending of the difficulty level
		if (DifficlutyLevel == 1) {
			showmax = 8;
		}
		else if (DifficlutyLevel == 2) {
			showmax = 6;
		}
		else {
			showmax = 4;
		}

		// set avalible cells array
		for (int i = 0; i < 25; i++) {
			if (i != 4) avaliblecells.Add(i);
		}

		// clear updating array
		Source->JigsawData.Empty();

		// fill array
		for (int i = 0; i < 25; i++) {
			if (i == 4) {
				b.CellIndex = 4;
				b.RotationState = 0;
				b.Location = GetPieceLocation(b.CellIndex);
				b.Rotation = FRotator::ZeroRotator.Quaternion();
			}
			else if (avaliblecells.Num() > 0) {
				if (avaliblecells.Num() > 0) {
					locindex = avaliblecells[FMath::RandRange(0, avaliblecells.Num() - 1)];
					avaliblecells.Remove(locindex);
				}
				else {
					locindex = -1;
				}

				b.CellIndex = locindex; // -1;
				b.RotationState = FMath::RandRange(0, 3);
				b.Location = GetPieceLocation(locindex);
				b.Rotation = FRotator(0.0f, b.RotationState * 90.0f, 0.0f).Quaternion();

			}
			b.IsVisible = (i >= 2 && i <= showmax); //false;
			Source->JigsawData.Add(b);
		}
	}
}

/* ------------------- SAVING TO DATA DEVICE ------------------- */

/* Load jigsaw information from parent Data Module */
void ADataJigsaw::LoadJigsaw()
{
	JigsawData.Empty();
	FJigsawCell a;
	a.AttachedIndex = -1; a.RotationState = -1;
	for (int i = 0; i < 25; i++) {
		JigsawData.Add(a);
	}

	if (IsValid(ParentDataDevice)) {
		for (int i = 0; i < 25; i++) {
			if (ParentDataDevice->JigsawData.Num() > i) {
				// set collision and visibility
				JigsawPiece[i]->SetVisibility(ParentDataDevice->JigsawData[i].IsVisible);
				if (ParentDataDevice->JigsawData[i].IsVisible) {
					JigsawPiece[i]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				}
				else {
					JigsawPiece[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}

				// set transform
				JigsawPiece[i]->SetRelativeLocation(ParentDataDevice->JigsawData[i].Location);
				JigsawPiece[i]->SetRelativeRotation(ParentDataDevice->JigsawData[i].Rotation);

				// set cell state
				if (ParentDataDevice->JigsawData[i].CellIndex >= 0) {
					JigsawData[ParentDataDevice->JigsawData[i].CellIndex].AttachedIndex = i;
					JigsawData[ParentDataDevice->JigsawData[i].CellIndex].RotationState = ParentDataDevice->JigsawData[i].RotationState;
				}
			}
			else {
				JigsawPiece[i]->SetVisibility(false);
				JigsawPiece[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

/* Save jigsaw information to parent Data Module */
void ADataJigsaw::SaveJigsaw()
{
	if (IsValid(ParentDataDevice)) {
		ParentDataDevice->JigsawData.Empty();

		FJigsawElement piece;
		for (int i = 0; i < 25; i++) {
			// visibility
			piece.IsVisible = JigsawPiece[i]->IsVisible();

			// cell and cell rotation
			piece.CellIndex = -1;
			piece.RotationState = -1;
			for (int n = 0; n < 25; n++) {
				if (JigsawData[n].AttachedIndex == i) {
					piece.CellIndex = n;
					piece.RotationState = JigsawData[n].RotationState;
					break;
				}
			}

			// transform
			piece.Location = JigsawPiece[i]->GetRelativeTransform().GetLocation();
			piece.Rotation = JigsawPiece[i]->GetRelativeTransform().Rotator().Quaternion();

			ParentDataDevice->JigsawData.Add(piece);
		}
	}
}

/* ------------------- PIECES POSITIONING ------------------- */

/* Get Jigsaw 5x5 element location in local space */
FVector ADataJigsaw::GetPieceLocation(int32 Index)
{
	FVector vRet;
	float BodyWidth = JIGSAW_BodyWidth*JIGSAW_ScaleMultiplier;
	int a, b;

	a = Index % 5; // column
	b = (Index - a) / 5; // row

	vRet.X = a * (BodyWidth * 0.2f) - (BodyWidth * 0.4f);
	vRet.Y = b * (BodyWidth * 0.2f) - (BodyWidth * 0.4f);
	vRet.Z = 0.1f;

	return vRet;
}

void ADataJigsaw::CorrectPieceLocation(int32 Index)
{
	float PieceAngles[8];
	float BodyWidth = JIGSAW_BodyWidth*JIGSAW_ScaleMultiplier;

	FVector itemloc = JigsawPiece[Index]->GetRelativeTransform().GetLocation();
	FVector jigsawloc;
	FVector offset = JigsawPiece[Index]->GetComponentLocation() - GetActorLocation();
	float dist = BodyWidth * 0.1f;

	// check if piece is outside of the jigsaw
	if (FMath::Abs(offset.Z) > BodyWidth * 0.2f || offset.Size() > BodyWidth * 0.7f) return;

	// init jigsaw angles
	PieceAngles[0] = GetActorRotation().Yaw - 360.0f;
	for (int i = 1; i < 8; i++)
		PieceAngles[i] = PieceAngles[i - 1] + 90.0f; // two periods -360..360

	// relocate piece
	for (int i = 0; i < 25; i++) {
		jigsawloc = GetPieceLocation(i); // need world?
		offset = jigsawloc - itemloc;

		if (offset.Size() <= dist && JigsawData[i].AttachedIndex == -1) {
			JigsawPiece[Index]->SetRelativeLocation(jigsawloc);
			JigsawData[i].AttachedIndex = Index;

			float yaw = JigsawPiece[Index]->GetComponentRotation().Yaw;
			for (int n = 0; n < 8; n++) {
				if (FMath::Abs(PieceAngles[n] - yaw) < 45.0f) {
					JigsawPiece[Index]->SetRelativeRotation(FRotator(0.0f, n * 90.0f, 0.0f));
					JigsawData[i].RotationState = n % 4;
					break;
				}
			}

			break;
		}
	}

	// check if puzzle is complete
	if (IsValid(ParentDataDevice)) {
		int32 nRightElements = 0;
		for (int i = 0; i < 25; i++) {
			if (JigsawData[i].AttachedIndex == i && JigsawData[i].RotationState == 0) {
				nRightElements++;
			}
		}

		if (nRightElements == 25) {
			ParentDataDevice->OnJigsawComplete();
		}
		else {
			ParentDataDevice->OnJigsawPieceComplete(nRightElements);
		}
	}
}

/* ------------------- ATTACHING TO CONTROLLER ------------------- */

void ADataJigsaw::AttachPieceToController(AHandController* Controller, int32 Index)
{
	JigsawPiece[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Controller->TakeStaticMesh(JigsawPiece[Index], nullptr);
	Controller->CurrentJigsaw = this;
	HighlightedPiece = -1;

	// reset cell state
	for (int i = 0; i < 25; i++) {
		if (JigsawData[i].AttachedIndex == Index) {
			JigsawData[i].AttachedIndex = -1;
			JigsawData[i].RotationState = -1;
			break;
		}
	}
}

void ADataJigsaw::DetachPieceFromController(AHandController* Controller, int32 Index)
{
	Controller->ReleaseStaticMesh();
	JigsawPiece[Index]->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	JigsawPiece[Index]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CorrectPieceLocation(Index);
	HighlightedPiece = Index;
}

/* ------------------- CONTROLLER TRIGGER ------------------- */

void ADataJigsaw::OnControllerTriggerPressed(AHandController* Controller)
{
	if (HighlightedPiece >= 0 && HighlightedPiece != 4) {
		AttachPieceToController(Controller, HighlightedPiece);
	}
}

void ADataJigsaw::OnControllerTriggerReleased(AHandController* Controller)
{
	UMeshComponent* m = Controller->GetHoldingMesh();

	if (IsValid(m) && m->ComponentHasTag("jigsawpiece") && m->ComponentTags.Num() == 2) {
		int Index = FCString::Atoi(*m->ComponentTags[1].ToString());

		DetachPieceFromController(Controller, Index);
	}
}

/* ------------------- COLLIDER OVERLAPS ------------------- */

void ADataJigsaw::JigsawPiece_Overlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 Index)
{
	if (Index == 4) return;

	AHandController* ctrl = Cast<AHandController>(OtherActor);

	if (IsValid(ctrl)) {
		if (OtherComp->ComponentHasTag(TEXT("ControllerToolLine"))) {
			FVector v1 = ctrl->ToolMesh->GetSocketLocation(TEXT("HitSocket"));
			FVector v2 = JigsawPiece[Index]->GetComponentLocation();
			if ((v1 - v2).Size() > 15.0f) return;
		}
		ctrl->CurrentJigsaw = this;
		ctrl->SetToolTrace(false);

		if (HighlightedPiece >= 0 && HighlightedPiece <= 24) {
			mti_Piece[HighlightedPiece]->SetScalarParameterValue(TEXT("EmissionPower"), 1.0f);
		}
		mti_Piece[Index]->SetScalarParameterValue(TEXT("EmissionPower"), 7.0f);

		if (!ctrl->IsHoldingStaticMesh() && ctrl->ControllerTriggerState) {
			AttachPieceToController(ctrl, Index);
		}
		else {
			HighlightedPiece = Index;
		}
	}
}

void ADataJigsaw::JigsawPiece_EndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHandController::StaticClass())) {
		AHandController* ctrl = Cast<AHandController>(OtherActor);
		if (IsValid(ctrl) && !ctrl->IsHoldingStaticMesh()) ctrl->CurrentJigsaw = nullptr;

		if (HighlightedPiece >= 0 && HighlightedPiece <= 24) {
			mti_Piece[HighlightedPiece]->SetScalarParameterValue(TEXT("EmissionPower"), 1.0f);
		}

		HighlightedPiece = -1;
	}
}