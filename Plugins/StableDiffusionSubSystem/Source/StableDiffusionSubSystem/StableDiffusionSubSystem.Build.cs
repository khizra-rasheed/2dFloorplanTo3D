// Copyright by CodeParrots 2023. All Rights Reserved.

using UnrealBuildTool;

public class StableDiffusionSubSystem : ModuleRules
{
	public StableDiffusionSubSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;



        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("AssetTools");
            PrivateDependencyModuleNames.Add("Blutility");
            PrivateDependencyModuleNames.Add("MaterialEditor");
            PrivateDependencyModuleNames.Add("DesktopPlatform");
            PrivateDependencyModuleNames.Add("EditorScriptingUtilities");
        }

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore" , "HTTP","UMG",
                "MovieScene",
                "MovieSceneTracks",
                "PropertyPath",
                "TimeManagement",
                "JsonUtilities",
				"Json"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"DeveloperSettings",
				"CoreUObject",
                "Engine",
                "Slate",
				"SlateCore",
                "UMG",
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
