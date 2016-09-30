// Anton Antonov, 2016. All rights reserved.

#pragma once

#define JIGSAW_BodyWidth 100.0f
#define JIGSAW_ScaleMultiplier 1.0f

#include "GameFramework/Actor.h"
#include "HandController.h"
#include "DataJigsaw.generated.h"

class ADataDevice;

/* describes jigsaw cell state - for this class */
USTRUCT(BlueprintType, Blueprintable)
struct FJigsawCell
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttachedIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RotationState;
};

/* describes jigsaw piece state - for parent device class */
USTRUCT(BlueprintType, Blueprintable)
struct FJigsawElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CellIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RotationState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsVisible;
};

UCLASS(Blueprintable)
class UVRP_API ADataJigsaw : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADataJigsaw();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* BackgroundPlane;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* JigsawPiece[25];

	//---------------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void InitParentDataDevice(ADataDevice* Source) { ParentDataDevice = Source; };

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	static void ResetJigsaw(ADataDevice* Source, int32 DifficlutyLevel = 3);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void LoadJigsaw();

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void SaveJigsaw();

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void OnControllerTriggerPressed(AHandController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Jigsaw")
	void OnControllerTriggerReleased(AHandController* Controller);

protected:
	UPROPERTY()
	TArray<FJigsawCell> JigsawData;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> mti_Piece;

	UPROPERTY()
	int HighlightedPiece;

	UPROPERTY()
	ADataDevice* ParentDataDevice;

	//---------------------------------------------------------------------------------------------------------------------------

	UFUNCTION()
	void InitMaterials();

	UFUNCTION()
	static FVector GetPieceLocation(int32 Index);

	UFUNCTION()
	void CorrectPieceLocation(int32 Index);

	UFUNCTION()
	void AttachPieceToController(AHandController* Controller, int32 Index);

	UFUNCTION()
	void DetachPieceFromController(AHandController* Controller, int32 Index);

	UFUNCTION()
	void JigsawPiece_EndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void JigsawPiece_Overlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 Index);

	UFUNCTION()
	void JigsawPiece01_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 0); };
	UFUNCTION()
	void JigsawPiece02_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 1); };
	UFUNCTION()
	void JigsawPiece03_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 2); };
	UFUNCTION()
	void JigsawPiece04_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 3); };
	UFUNCTION()
	void JigsawPiece05_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 4); };
	UFUNCTION()
	void JigsawPiece06_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 5); };
	UFUNCTION()
	void JigsawPiece07_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 6); };
	UFUNCTION()
	void JigsawPiece08_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 7); };
	UFUNCTION()
	void JigsawPiece09_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 8); };
	UFUNCTION()
	void JigsawPiece10_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 9); };
	UFUNCTION()
	void JigsawPiece11_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 10); };
	UFUNCTION()
	void JigsawPiece12_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 11); };
	UFUNCTION()
	void JigsawPiece13_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 12); };
	UFUNCTION()
	void JigsawPiece14_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 13); };
	UFUNCTION()
	void JigsawPiece15_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 14); };
	UFUNCTION()
	void JigsawPiece16_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 15); };
	UFUNCTION()
	void JigsawPiece17_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 16); };
	UFUNCTION()
	void JigsawPiece18_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 17); };
	UFUNCTION()
	void JigsawPiece19_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 18); };
	UFUNCTION()
	void JigsawPiece20_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 19); };
	UFUNCTION()
	void JigsawPiece21_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 20); };
	UFUNCTION()
	void JigsawPiece22_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 21); };
	UFUNCTION()
	void JigsawPiece23_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 22); };
	UFUNCTION()
	void JigsawPiece24_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 23); };
	UFUNCTION()
	void JigsawPiece25_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { if (OtherActor->IsA(AHandController::StaticClass())) JigsawPiece_Overlap(OtherActor, OtherComp, 24); };
};
