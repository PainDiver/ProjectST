// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class STNet : ModuleRules
{
	public STNet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		///프로토버퍼용 
		string ProtobufLibraryPath = ModuleDirectory +  "/../../ThirdParty/Protobuf/lib";
		string ProtobufSourcePath = ModuleDirectory + "/../../Source/STNet/Public/ThirdParty/Protobuf";
		if (Target.Configuration == UnrealTargetConfiguration.Debug)
		{
			PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/libprotobufd.lib");
			RuntimeDependencies.Add(ProtobufLibraryPath + "libprotobufd.dll");
		}
		else
		{
			PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/libprotobuf.lib");
			RuntimeDependencies.Add(ProtobufLibraryPath + "libprotobuf.dll");
		}
		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/abseil_dll.lib");
		RuntimeDependencies.Add(ProtobufLibraryPath + "abseil_dll.dll");


		PublicIncludePaths.Add(ProtobufSourcePath);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"DeveloperSettings",
				"Networking",
				"Sockets",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
