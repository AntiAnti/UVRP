// Anton Antonov, 2016. All rights reserved.

#pragma once

#include "SteamConnection.generated.h"

/**
 * 
 */
UCLASS()
class UVRP_API USteamConnection : public UObject
{
	GENERATED_BODY()

public:
	USteamConnection();
	~USteamConnection();

	/*
protected:
	UFUNCTION()
	inline IOnlineSubsystem* InitSubsystem();
	*/

public:
	UFUNCTION()
	bool IsPlayerLoggedIn();

	UFUNCTION()
	void UnlockAchievement(EGameAchievements AchievementType);

	UFUNCTION()
	void LoadAchievementStatus(EGameAchievements AchievementType, bool& ResultVariable);

	UFUNCTION()
	void LoadLeaderboardData(int32 Position, FString& PlayerName, int32& PlayerScore) {};
};
