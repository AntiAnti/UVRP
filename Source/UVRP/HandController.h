// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HandController.generated.h"

class UUGunComponent;
class UArrowComponent;
class UGunSlingComponent;
class UControllerMenuPointer;
class UTeleportRingsSelector;
class ABomb;
class AUVRPCharacter;
class ADataDevice;
class ADataJigsaw;

UENUM(BlueprintType, Blueprintable)
enum class EControllerTool : uint8
{
	C_Tool			UMETA(DisplayName = "Tool"),
	C_Gun_Small		UMETA(DisplayName = "Gun Small"),
	C_Gun_Large		UMETA(DisplayName = "Gun Large"),
	C_Shield		UMETA(DisplayName = "Shield"),
	C_Container		UMETA(DisplayName = "Container"),
	C_Gun_Sling		UMETA(DisplayName = "Gun Sling"),
	C_Additional	UMETA(DisplayName = "Additional"),
	C_ModuleRed		UMETA(DisplayName = "Module Red"),
	C_ModuleBlue	UMETA(DisplayName = "Module Blue"),
	C_ModuleGreen	UMETA(DisplayName = "Module Green"),
	C_ModuleViolet	UMETA(DisplayName = "Module Violet"),
	C_None			UMETA(DisplayName = "None")
};

UENUM(BlueprintType, Blueprintable)
enum class EHandSide : uint8
{
	S_Right			UMETA(DisplayName = "Right"),
	S_Left			UMETA(DisplayName = "Left")
};

UENUM(BlueprintType, Blueprintable)
enum class EControllerButtonType : uint8
{
	S_Trigger		UMETA(DisplayName = "Trigger"),
	S_Drop			UMETA(DisplayName = "Drop")
};

UCLASS(Blueprintable)
class UVRP_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	/************************ COMPONENTS ************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRootComponent;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* MainRootArrow;
	*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUGunComponent* GunSmall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ToolBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ToolMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UControllerMenuPointer* ToolPointer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* ToolCollider;
	
	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* HandSocket;
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUGunComponent* GunLarge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* ShieldMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* HitMenuCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* PositionalTrigger1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* PositionalTrigger2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* PositionalTrigger3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGunSlingComponent* GunSuper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ToolMenuMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* HandStaticMesh;

	/************************ VARIABLES ************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data Device")
	int32 CurrentDataDevice;

	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsGunSmallActive;
	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsGunLargeActive;
	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsGunSuperActive;
	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsToolActive;
	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsShieldActive;
	UPROPERTY(BlueprintReadWrite, Category = "Tools")
	bool IsAdvancedShieldActive;

	UPROPERTY()
	ABomb* CurrentBomb;

	UPROPERTY()
	ADataJigsaw* CurrentJigsaw;

	UPROPERTY()
	USoundBase* OnClickSound;

	UPROPERTY()
	USoundBase* OnThrowSound;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global")
	EControllerTool ControllerMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global")
	FVector InitialOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global")
	UTeleportRingsSelector* RingsSelectorActor;

	UPROPERTY()
	bool ControllerTriggerState;

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetHandSide(EHandSide NewSide);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void UpdateControllerMode(EControllerTool NewMode);

	UFUNCTION()
	FTransform GetHandTransform();

	UFUNCTION()
	FRotator GetToolRotation();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	AActor* GetToolEventRecipient();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActoinFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActionShiftMove(bool Direction);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetToolTrace(bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetupGunProjectileClass(TSubclassOf<class AUVRPProjectile> ProjClass);

	UFUNCTION()
	void OnBeginOverlap_MenuCollider(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap_MenuCollider(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	FVector GetToolColliderAimTarget();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool TakeStaticMesh(UMeshComponent* MainMesh, USceneComponent* AdditionalMesh);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void PutStaticMesh();	// drop it

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ReleaseStaticMesh(); // release without changing physics state

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsHoldingStaticMesh() { return (HoldingMesh1 != nullptr); };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool HandleOnClickEvent(EControllerButtonType Button);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool AcceptDataDeviceAttaching() { return (EControllerTool::C_Tool == ControllerMode && CurrentDataDevice == -1); };

	UFUNCTION()
	void TimerUpdateFunction();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ActivateHandComponent(EControllerTool Component);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetAnotherController(AHandController* Controller) { AnotherController = Controller; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	FVector GetToolPointerBlockingPoint();

	UFUNCTION(BlueprintCallable, Category = "Tool Menu")
	void SetToolMenuVisibility(bool IsVisible);

	UFUNCTION(BlueprintCallable, Category = "Tool Menu")
	void SetToolMenuActiveSlot(FVector2D State);

	UFUNCTION(BlueprintCallable, Category = "Tool Menu")
	void SetToolMenuDisplay(bool ShowDataModulesMenu);

	UFUNCTION(BlueprintCallable, Category = "Tool Menu")
	EControllerTool GetCurrentToolMenuSlot(uint8 CustomSlot = 255);

	UFUNCTION(BlueprintCallable, Category = "Hand")
	void SetHandVisibility(bool IsVisible) { HandStaticMesh->SetVisibility(IsVisible); };

	UFUNCTION(BlueprintCallable, Category = "Hand")
	bool GetHandVisibility() { return HandStaticMesh->IsVisible(); };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetShieldColor(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsRightHand() { return HandSide == EHandSide::S_Right; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	UMeshComponent* GetHoldingMesh() { return HoldingMesh1; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnTriggerStateChanged(bool IsPressedOrReleased);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsConnectedToLock() { return bConnectedToLock; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Global")
	EHandSide HandSide;

	UPROPERTY()
	UAnimSequence* anim_ShieldOpen; //AnimSequence'/Game/FirstPerson/Meshes/anim_PlayerShield_Open.anim_PlayerShield_Open'

	UPROPERTY()
	UAnimSequence* anim_ShieldClose; //AnimSequence'/Game/FirstPerson/Meshes/anim_PlayerShield_Close.anim_PlayerShield_Close'

	UPROPERTY()
	UMaterialInstanceDynamic* mti_ShieldField;

	UPROPERTY()
	uint8 IsShieldAnimationFinished;

	UPROPERTY()
	FTimerHandle hMainTimer;

	UPROPERTY()
	FTimerHandle hShieldTimer;

	UPROPERTY()
	FTimerHandle hThrowStaticMeshTimer;

	UPROPERTY()
	float PrevTimerTime;

	UPROPERTY()
	AHandController* AnotherController;

	UPROPERTY()
	UMeshComponent* HoldingMesh1;

	UPROPERTY()
	USceneComponent* HoldingMesh2;

	UPROPERTY()
	UMeshComponent* ThrowStaticMesh;

	UPROPERTY()
	FVector PreviousSocketLocation;

	UPROPERTY()
	FVector FireSocketSpeed;

	UPROPERTY()
	UMaterialInstanceDynamic* mti_ToolMenu;

	UPROPERTY()
	UTexture2D* tx_ToolMenuDisplay1;

	UPROPERTY()
	UTexture2D* tx_ToolMenuDisplay2;

	UPROPERTY()
	bool CurrentToolMenuDisplay;

	UPROPERTY()
	uint8 ToolMenuActiveSlot;

	UPROPERTY()
	AUVRPCharacter* PlayerOwner;

	UPROPERTY()
	UStaticMesh* LeftHandStaticMesh;

	UPROPERTY()
	UStaticMesh* RightHandStaticMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* mti_Shield;

	UPROPERTY()
	bool bConnectedToLock;

	/*
	UPROPERTY()
	ADataDevice* BeamDataModule;
	*/

protected:
	UFUNCTION()
	void Timer_ReactivateController();

	UFUNCTION()
	void Timer_ThrowStaticMesh();

	UFUNCTION()
	void ThrowIntroLevelBoxCover();

	UFUNCTION()
	void SetupVisibleDataDevice(int32 DataMuduleNumber);

private:
	bool TriggersState[3];

	UFUNCTION()
	void SetTriggerValue(int32 Index, bool Value, class AActor* OtherActor, class UPrimitiveComponent* OtherComp);

	UFUNCTION()
	void Trigger1_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(0, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger1_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(0, false, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger2_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(1, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger2_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(1, false, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger3_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { SetTriggerValue(2, true, OtherActor, OtherComp); };
	UFUNCTION()
	void Trigger3_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { SetTriggerValue(2, false, OtherActor, OtherComp); };
	UFUNCTION()
	void OnShieldHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
