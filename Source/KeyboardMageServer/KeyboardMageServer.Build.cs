// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KeyboardMageServer : ModuleRules
{
    public KeyboardMageServer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "KeyboardMage",
			"Core",
            "CoreUObject",
            "Engine",
			"NetCore",
            "Networking",
            "Sockets"
        });
/*
        PublicIncludePaths.AddRange(new string[] {
            "KeyboardMage",
            "KeyboardMage/BaseGame",
            "KeyboardMageServer",
            "KeyboardMageServer/BaseGame"
        });
*/
    }
}