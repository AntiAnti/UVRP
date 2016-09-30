// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HandMenuEmpty.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EMenuItemName : uint8
{
	MI__ROOT			UMETA(DisplayName = "ROOT"),
	MI_NewGame			UMETA(DisplayName = "NewGame"),
	MI_Options 			UMETA(DisplayName = "Options"),
	MI_Statistics		UMETA(DisplayName = "Statistics"),
	MI_Learning			UMETA(DisplayName = "Learning"),
	MI_MainMenu			UMETA(DisplayName = "MainMenu"),
	MI_SetupHands		UMETA(DisplayName = "SetupHands"),
	MI_SetupSound		UMETA(DisplayName = "SetupSound"),
	MI_SetupDiffLevel	UMETA(DisplayName = "SetupDiffLevel"),
	MI_SetupVolume		UMETA(DisplayName = "SetupVolume"),
	MI_SetupGraphics	UMETA(DisplayName = "SetupGraphics"),
	MI_Continue			UMETA(DisplayName = "Continue"),
	MI_Cancel			UMETA(DisplayName = "Cancel"),
	MI_Back				UMETA(DisplayName = "Back"),
	MI_Exit				UMETA(DisplayName = "Exit"),
	MI_SetupVoice		UMETA(DisplayName = "SetupVoice"),
	MI_SetupMusic		UMETA(DisplayName = "SetupMusic"),
	MI_Manual			UMETA(DisplayName = "Manual"),
	MI_Automatic		UMETA(DisplayName = "Automatic"),
	MI_Subtitles		UMETA(DisplayName = "Subtitles"),
	MI_On				UMETA(DisplayName = "On"),
	MI_Off				UMETA(DisplayName = "Off"),
	MI_Floor			UMETA(DisplayName = "Floor")
};

UENUM(BlueprintType, Blueprintable)
enum class EMenuItemType : uint8
{
	IT_Action			UMETA(DisplayName = "Action"),
	IT_Folder			UMETA(DisplayName = "Folder"),
	IT_Title			UMETA(DisplayName = "Title"),
	IT_Parameter		UMETA(DisplayName = "Parameter"),
	IT_Back				UMETA(DisplayName = "Back")
};

USTRUCT(BlueprintType, Blueprintable)
struct FMenuTreeItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ParentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuItemName Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuItemType Type;
};

UCLASS(Blueprintable)
class UVRP_API AHandMenuEmpty : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandMenuEmpty();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* PlaneRoot;

	/*
	UFUNCTION(BlueprintCallable, Category = "Main")
	virtual void Show();

	UFUNCTION(BlueprintCallable, Category = "Main")
	virtual void Hide();
	*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TArray<FMenuTreeItem> MenuTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Global")
	int32 MenuPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	UMaterialInstance *MainMaterialInstance;

	UFUNCTION(Category = "Events")
	virtual float GetItemHeight();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ShowFinished();
	virtual void ShowFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void HideFinished();
	virtual void HideFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnOpenItem(FMenuTreeItem item);
	virtual void OnOpenItem_Implementation(FMenuTreeItem item);

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnExitMenu();
	virtual void OnExitMenu_Implementation();

protected:
	UPROPERTY()
	FTimerHandle hShowHide;
	UPROPERTY()
	float fTargetOpacity;

public:
	UFUNCTION()
	void TimerSetOpacity();
};
