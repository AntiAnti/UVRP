// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/*/

USTRUCT()
struct FGamePlatformStatus
{
	GENERATED_USTRUCT_BODY()
	
	FString PlatformName;
	bool IsProtected;
	bool IsTeleportActive;
	bool IsAtttached;

	FGamePlatformStatus() {
		PlatformName = ""; IsProtected = true; IsTeleportActive = false; IsAtttached = false;
	}
};
*/
