// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class UVRP : ModuleRules
{
    public UVRP(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "SteamVR", "SteamVRController" });

        PrivateDependencyModuleNames.AddRange(new string[] {  });

        //PublicAdditionalLibraries.Add("steam_api64.lib");

        //DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //PublicAdditionalLibraries.Add("Engine\\Binaries\\ThirdParty\\Steamworks\\Steamv137\\Win64\\steam_api64.lib");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
         PrivateDependencyModuleNames.AddRange(new string[] {
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
            }
        );
        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
         {
        		if (UEBuildConfiguration.bCompileSteamOSS == true)
        		{
        			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        		}
         }
    }
}
