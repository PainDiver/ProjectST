// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class STNet : ModuleRules
{
	public STNet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


		///프로토버퍼용 
		string PluginPath = ModuleDirectory + "../../../";
		string ProtobufLibraryPath = PluginPath + "ThirdParty/Protobuf/lib";
		string ProtobufSourcePath = PluginPath + "Source/STNet/Public/ThirdParty/Protobuf";
		string PlatformPath = PluginPath + "/Binaries/Win64";

		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/libprotobuf.lib");
		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/abseil_dll.lib");
		PublicAdditionalLibraries.Add(ProtobufLibraryPath + "/zlib.lib");

		
		RuntimeDependencies.Add(PlatformPath + "/libprotobuf.dll", ProtobufLibraryPath + "/libprotobuf.dll");
		RuntimeDependencies.Add(PlatformPath + "/zlib1.dll", ProtobufLibraryPath + "/zlib1.dll");
		RuntimeDependencies.Add(PlatformPath + "/abseil_dll.dll", ProtobufLibraryPath + "/abseil_dll.dll");


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
