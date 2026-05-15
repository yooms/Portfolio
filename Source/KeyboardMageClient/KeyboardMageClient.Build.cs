// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KeyboardMageClient : ModuleRules
{
    public KeyboardMageClient(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "KeyboardMage",
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
	        "Slate",
            "SlateCore",
			"UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ApplicationCore"
        });

/*
        PublicIncludePaths.AddRange(new string[] {
            "KeyboardMage",
            "KeyboardMage/BaseGame",
            "KeyboardMageClient",
            "KeyboardMageClient/BaseGame"
        });
*/
    }
}