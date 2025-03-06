// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class ProjectST : ModuleRules
{
	public ProjectST(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { 
			ModuleDirectory,
			"../Plugins/STUISystem/Source/STUISystem/Public",
			"../Plugins/STNet/Source/STNet/Public"
		});

		PrivateIncludePaths.AddRange(new string[] {
			"../Plugins/STUISystem/Source/STUISystem/Private",
			"../Plugins/STNet/Source/STNet/Private"
		});
		
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"STNet",
			"STUISystem",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"NetCore",
			"Json",
			"JsonUtilities",
		});
	}
}
