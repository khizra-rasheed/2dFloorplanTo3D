// Copyright by CodeParrots 2023. All Rights Reserved.

using UnrealBuildTool;

public class StableDiffusionSubSystemEditor : ModuleRules
{
    public StableDiffusionSubSystemEditor(ReadOnlyTargetRules Target) : base(Target)
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

        PrivateIncludePaths.AddRange(new string[] { System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "Source/Editor/Blutility/Private" });


        PublicDependencyModuleNames.AddRange(
        new string[]
        {
                    "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore" , "HTTP","UMG",
                    "MovieScene",
                    "MovieSceneTracks",
                    "PropertyPath",
                    "TimeManagement",
                    "DesktopPlatform",
                    "JsonUtilities",
                    "Json","StableDiffusionSubSystem"
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
                "DesktopPlatform",
                "SlateCore",
                "UMG","StableDiffusionSubSystem",
				// ... add private dependencies that you statically link with here ...	
			}
            );
    }
}
