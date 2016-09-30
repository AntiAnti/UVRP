// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "DataModuleViewList.generated.h"

class AHandController;
class ADataDevice;
class AMessageWindow;

UENUM(BlueprintType, Blueprintable)
enum class EDataModuleItemType : uint8
{
	DT_Folder			UMETA(DisplayName = "Folder"),
	DT_TextFile			UMETA(DisplayName = "TextFile"),
	DT_Back				UMETA(DisplayName = "Back")
};

USTRUCT(BlueprintType, Blueprintable)
struct FDataModuleItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ParentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDataModuleItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TargetFile;
};

UCLASS(Blueprintable)
class UVRP_API ADataModuleViewList : public AActor, public IMenuInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataModuleViewList();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* ListMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* ListCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshSelector;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FDataModuleItem> ListData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FDataModuleItem> ActiveList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	ADataDevice* DataDeviceOwner;

	UPROPERTY()
	AMessageWindow* CurrentFileWindow;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION(BlueprintCallable, Category = "DataModuleViewList")
	void Navigate(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "DataModuleViewList")
	void DestroyList();

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool OnClick(FVector HitPoint) override;

	UFUNCTION(BlueprintCallable, Category = "DataModuleViewList")
	void OnDataWindowClosed();
	
protected:
	UPROPERTY()
	int32 ActiveListIndex;

	UPROPERTY()
	FTimerHandle hTimerUpdateSelector;

	UPROPERTY()
	AHandController* ActiveController;

	UFUNCTION()
	void UpdateSelectorView();

public:
	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
