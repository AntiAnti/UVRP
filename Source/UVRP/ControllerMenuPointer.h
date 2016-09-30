// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#ifndef _ControllerMenuPoriner_MenuInteractionInterface
#include "MenuInteractionInterface.h"
#define _ControllerMenuPoriner_MenuInteractionInterface 1
#endif
#include "ControllerMenuPointer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UVRP_API UControllerMenuPointer : public UStaticMeshComponent
{
	GENERATED_BODY()

	UControllerMenuPointer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetLength(float Length);

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool OnClickEvent() {
		if (CurrentMenuItem != nullptr) {
			//CurrentMenuItem->OnClick(CurrentMenuItemName_Comp);
			//CurrentMenuItem->BP_OnClick.Broadcast(CurrentMenuItemName_Comp);
			return true;
		} else return false;
	};

	UFUNCTION()
	void TracePlayerSelectorLine(float DeltaTime);

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor *CurrentEventRecipient;

	UPROPERTY()
	FVector TraceBlockingPoint;

	UFUNCTION()
	void AddIgnoredComponent(UPrimitiveComponent* Component);

private:
	UPROPERTY()
	float fLineLength;

	UPROPERTY()
	FString CurrentMenuItemName_Actor;
	UPROPERTY()
	FString CurrentMenuItemName_Comp;

	UPROPERTY()
	TScriptInterface<IMenuInteractionInterface> CurrentMenuItem;

	FCollisionQueryParams stTraceParams;
};
