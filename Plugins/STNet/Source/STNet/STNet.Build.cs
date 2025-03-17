// Copyright Epic Games, Inc. All Rights Reserved.

using Microsoft.CodeAnalysis;
using UnrealBuildTool;

public class STNet : ModuleRules
{
	public STNet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		///프로토버퍼용 

		string PluginPath = ModuleDirectory + "/../../";
		string ProtobufLibraryPath = PluginPath + "ThirdParty/Protobuf/lib";
		string ProtobufSourcePath = PluginPath + "Source/STNet/Public/ThirdParty/Protobuf";
		string ProjectPlatformPath = PluginPath + $"../../Binaries/{Target.Platform.ToString()}";
		string PluginPlatformPath = PluginPath + $"Binaries/{Target.Platform.ToString()}";

		PublicIncludePaths.Add(ProtobufSourcePath);

		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/libprotobuf.lib");
		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/abseil_dll.lib");
		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/zlib.lib");

		if (Target.Type == TargetType.Editor)
		{
			RuntimeDependencies.Add(PluginPlatformPath + "/libprotobuf.dll", ProtobufLibraryPath + "/libprotobuf.dll");
			RuntimeDependencies.Add(PluginPlatformPath + "/zlib1.dll", ProtobufLibraryPath + "/zlib1.dll");
			RuntimeDependencies.Add(PluginPlatformPath + "/abseil_dll.dll", ProtobufLibraryPath + "/abseil_dll.dll");
		}
		else
		{
			RuntimeDependencies.Add(ProjectPlatformPath + "/libprotobuf.dll", ProtobufLibraryPath + "/libprotobuf.dll");
			RuntimeDependencies.Add(ProjectPlatformPath + "/zlib1.dll", ProtobufLibraryPath + "/zlib1.dll");
			RuntimeDependencies.Add(ProjectPlatformPath + "/abseil_dll.dll", ProtobufLibraryPath + "/abseil_dll.dll");
		}

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
