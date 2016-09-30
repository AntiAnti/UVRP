// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UVRPHUD.generated.h"

UCLASS()
class AUVRPHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUVRPHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

