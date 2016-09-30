// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class UVRPEditorTarget : TargetRules
{
	public UVRPEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
        bUsesSteam = true;
    }

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "UVRP" } );
	}
}
