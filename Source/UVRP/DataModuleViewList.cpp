// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "HandController.h"
#include "DrawDebugHelpers.h"
#include "UVRPGameMode.h"
#include "DataDevice.h"
#include "MessageWindow.h"
#include "DataModuleViewList.h"

// Sets default values
ADataModuleViewList::ADataModuleViewList()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ListMesh = CreateDefaultSubobject<UTextRenderComponent>("ListMesh");
	RootComponent = ListMesh;
	ListMesh->XScale = TEXT_RENDER_SIZE;
	ListMesh->YScale = TEXT_RENDER_SIZE;
	ListMesh->WorldSize = 10.0f;
	ListMesh->SetRelativeLocation(FVector(0.0f, 30.0f, 0.0f));
	ListMesh->SetRelativeRotation(FRotator::ZeroRotator);
	ListMesh->SetText(FText::FromString("[DATA LIST]"));
	ListMesh->SetVisibility(true);

	static ConstructorHelpers::FObjectFinder<UFont> fntMain(TEXT("Font'/Game/FirstPerson/Effects/RobotoDistanceField_2.RobotoDistanceField_2'"));
	if (fntMain.Object != NULL) ListMesh->SetFont(fntMain.Object);
	static ConstructorHelpers::FObjectFinder<UMaterial> mtlMain(TEXT("Material'/Game/FirstPerson/Shaders/DefaultTextMaterialOpaque2.DefaultTextMaterialOpaque2'"));
	if (mtlMain.Object != NULL) ListMesh->SetTextMaterial(mtlMain.Object);

	ListCollider = CreateDefaultSubobject<UBoxComponent>("ListCollider");
	ListCollider->SetupAttachment(GetRootComponent());
	ListCollider->SetRelativeLocation(FVector::ZeroVector);
	ListCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ListCollider->SetCollisionObjectType(ECC_ControllerResponses);
	ListCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ListCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ListCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	ListCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ListCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ListCollider->bHiddenInGame = true;
	ListCollider->SetBoxExtent(FVector::ZeroVector);
	ListCollider->OnComponentBeginOverlap.AddDynamic(this, &ADataModuleViewList::OnBeginOverlap);
	ListCollider->OnComponentEndOverlap.AddDynamic(this, &ADataModuleViewList::OnEndOverlap);

	MeshSelector = CreateDefaultSubobject<UStaticMeshComponent>("MeshSelector");
	MeshSelector->SetupAttachment(GetRootComponent());
	MeshSelector->SetRelativeLocation(FVector::ZeroVector);
	MeshSelector->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	MeshSelector->SetVisibility(false);
	MeshSelector->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAssetPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/plane100x100.plane100x100'"));
	if (smAssetPlane.Object != NULL && IsValid(MeshSelector)) {
		MeshSelector->SetStaticMesh(smAssetPlane.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> mtiSelector(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_TNeon_Blue.mti_TNeon_Blue'"));
	if (mtiSelector.Object != NULL && IsValid(MeshSelector)) {
		MeshSelector->SetMaterial(0, mtiSelector.Object);
	}

	FDataModuleItem p1, p2, p3, p4, p5;
	p1.ParentIndex = -1;
	p1.Type = EDataModuleItemType::DT_Folder;
	p1.Text = "ROOT";
	p2.ParentIndex = 0;
	p2.Type = EDataModuleItemType::DT_Back;
	p2.Text = "..";
	p3.ParentIndex = 0;
	p3.Type = EDataModuleItemType::DT_Folder;
	p3.Text = "2484 Week 06";
	p4.ParentIndex = 0;
	p4.Type = EDataModuleItemType::DT_TextFile;
	p4.Text = "readme.txt";
	p5.ParentIndex = 2;
	p5.Type = EDataModuleItemType::DT_Back;
	p5.Text = "..";
	ListData.Add(p1);
	ListData.Add(p2);
	ListData.Add(p3);
	ListData.Add(p4);
	ListData.Add(p5);
}

// Called when the game starts or when spawned
void ADataModuleViewList::BeginPlay()
{
	Super::BeginPlay();

	Navigate(0);
}

// Called every frame
void ADataModuleViewList::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ADataModuleViewList::Navigate(int32 Index)
{
	if (Index < 0) { DestroyList(); return; }
	if (ListData.Num() == 0 || Index >= ListData.Num()) return;

	// open folder
	if (ListData[Index].Type == EDataModuleItemType::DT_Folder) {
		// fill active list
		ActiveList.Empty();
		int p = 0;
		for (int i = 0; i < ListData.Num(); i++) {
			if (ListData[i].ParentIndex == Index) {
				ActiveList.Add(ListData[i]);
				ActiveList[p++].ParentIndex = i;
			}
		}

		// fill text
		FString buffer = "";
		for (int i = 0; i < ActiveList.Num(); i++) {
			if (ActiveList[i].Type == EDataModuleItemType::DT_Folder)
				buffer += "> [" + ActiveList[i].Text + "]";
			else 
				buffer += ActiveList[i].Text;
			if (i < ActiveList.Num() - 1) buffer += "\n";
		}
		ListMesh->SetText(FText::FromString(buffer));
		ListMesh->UpdateBounds();

		// update components
		FVector datasize = ListMesh->GetTextLocalSize();
		/*float p = datasize.X;
		datasize.X = datasize.Y; datasize.Y = p;*/
		datasize.X = 0.0f; datasize.Y /= -2.0f; datasize.Z /= 2.0f;

		ListCollider->SetRelativeLocation(datasize);
		ListCollider->SetBoxExtent(FVector(0.2f, FMath::Abs(datasize.Y), datasize.Z));

		datasize.X = -0.5f;
		MeshSelector->SetRelativeLocation(datasize);
		datasize.X = 1.0f;
		datasize.Y = FMath::Abs(datasize.Y) / 50.0f;
		datasize.Z /= (50.0f * ActiveList.Num());

		MeshSelector->SetRelativeScale3D(datasize);
		//UpdateSelectorView();
	}
	else if (ListData[Index].Type == EDataModuleItemType::DT_Back) {
		if (ListData[Index].ParentIndex < 0)
			Navigate(-1);
		else {
			Navigate(ListData[ListData[Index].ParentIndex].ParentIndex);
		}
	}
	else if (ListData[Index].Type == EDataModuleItemType::DT_TextFile) {
		if (IsValid(DataDeviceOwner)) {
			AUVRPGameMode *gm = Cast<AUVRPGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(gm)) {
				FTransform tr1, tr2;
				FRotator r;
				bool IsAttached;
				IsAttached = DataDeviceOwner->IsDeviceAttached();

				if (IsAttached) {
					/*
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "DataDeviceOwner->IsAttachedToController() == true");
					tr2 = DataDeviceOwner->GetActorTransform();
					r = DataDeviceOwner->GetActorRotation();
					r.Yaw += 180.0f;
					r.Pitch += 0.0f;
					r.Roll += 180.0f;
					tr2.SetRotation(r.Quaternion());
					tr2.SetLocation(DataDeviceOwner->GetActorLocation() + DataDeviceOwner->GetActorUpVector() * 80.0f);
						
					tr1 = tr2;
					AMessageWindow* msg = gm->ShowDataWindow(DataDeviceOwner->UniqueNumber, FName(*ListData[Index].TargetFile), DataDeviceOwner, tr1);
					if (msg != nullptr) {
						msg->SetActorTransform(tr2);
						msg->SetOwnerMenu(this);
						GetRootComponent()->SetVisibility(false, true);
						ListCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						CurrentFileWindow = msg;
					}
					*/
					gm->ShowMessage(FText::FromString("To read files put the device on the floor."));
				}
				else {
					tr2 = this->GetActorTransform();
					r = tr2.GetRotation().Rotator();
					r.Yaw += 180.0f;
					tr2.SetLocation(DataDeviceOwner->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f));
					tr2.SetRotation(r.Quaternion());

					tr1 = tr2;
					AMessageWindow* msg = gm->ShowDataWindow(DataDeviceOwner->UniqueNumber, FName(*ListData[Index].TargetFile), DataDeviceOwner, tr1);
					if (msg != nullptr) {
						msg->SetActorTransform(tr2);
						msg->SetOwnerMenu(this);
						GetRootComponent()->SetVisibility(false, true);
						ListCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						CurrentFileWindow = msg;
					}
				}
			}
		}
	}	
}

void ADataModuleViewList::DestroyList()
{
	if (IsValid(CurrentFileWindow))
		CurrentFileWindow->Destroy();

	this->Destroy();
}

bool ADataModuleViewList::OnClick(FVector HitPoint)
{
	bool bRet = false;
	if (ActiveListIndex >= 0 && ActiveListIndex < ActiveList.Num() && MeshSelector->IsVisible()) {
		Navigate(ActiveList[ActiveListIndex].ParentIndex);
	}
	return bRet;
}

void ADataModuleViewList::OnDataWindowClosed()
{
	GetRootComponent()->SetVisibility(true, true);
	ListCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	CurrentFileWindow = nullptr;
}

void ADataModuleViewList::UpdateSelectorView()
{
	if (!IsValid(ActiveController)) return;

	FVector point = ActiveController->GetToolPointerBlockingPoint();
	float a, b, c, p, h, l, datf;
	FVector rt, lt, lb;
	FVector listsize = ListMesh->GetTextLocalSize();
	FRotator listrot = ListMesh->GetComponentRotation();
	FVector listloc = ListMesh->GetComponentLocation();

	lb = listloc;
	rt = listloc + ((ListCollider->GetComponentLocation()) - listloc) * 2.0f;
	lt = listloc + listrot.RotateVector(FVector(0.0f, 0.0f, 1.0f)) * listsize.Z;
	
	/*
	DrawDebugLine(GetWorld(), rt, lt, FColor::White, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), rt, lb, FColor::White, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), rt, point, FColor::White, false, 2.0f, 0, 1.0f);
	*/

	// треугольник rt - lt - point
	a = (point - rt).Size();
	b = (point - lt).Size();
	c = (lt - rt).Size();
	p = (a + b + c) / 2.0f;

	h = 2 * FMath::Sqrt(p * (p - a) * (p - b) * (p - c)) / c;
	l = (lb - lt).Size();

	datf = h / l;
	int index = (int)(datf * (float)ActiveList.Num());
	if (index >= ActiveList.Num()) index = ActiveList.Num() - 1;
	float heig = l / (float)ActiveList.Num();
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::FromInt(index));
	point.X = -0.5f; point.Y = -listsize.Y / 2.0f; point.Z = listsize.Z - heig * ((float)index + 0.5f);

	MeshSelector->SetRelativeLocation(point);
	ActiveListIndex = index;
}

void ADataModuleViewList::OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AHandController::StaticClass())) {
		ActiveController = Cast<AHandController>(OtherActor);
		
		GetWorldTimerManager().SetTimer(hTimerUpdateSelector, this, &ADataModuleViewList::UpdateSelectorView, 0.05f, true);
		UpdateSelectorView();
		MeshSelector->SetVisibility(true);
	}
}

void ADataModuleViewList::OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(hTimerUpdateSelector);
	MeshSelector->SetVisibility(false);
	//ActiveController = nullptr;
}