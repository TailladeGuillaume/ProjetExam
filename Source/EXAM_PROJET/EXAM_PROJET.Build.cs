// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EXAM_PROJET : ModuleRules
{
	public EXAM_PROJET(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
