// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KeyboardMage : ModuleRules
{
    public KeyboardMage(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
			"NetCore",
            "Networking",
            "EnhancedInput",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "Sockets",

            // Animation
            "AnimationCore",
            "AnimGraphRuntime",

            // Motion Matching – PoseSearch plugin
            // (Enable "PoseSearch" plugin in your .uproject)
            "PoseSearch",

            // Trajectory prediction – MotionTrajectory plugin
            // (Enable "MotionTrajectory" plugin in your .uproject)
            "MotionTrajectory",

            // Chooser (optional database filtering)
            // (Enable "Chooser" plugin in your .uproject)
            "Chooser",

        });

/*
        PrivateIncludePaths.AddRange(new string[] {
            "KeyboardMage",
            "KeyboardMage/BaseGame",
            "KeyboardMage/Character",
            "KeyboardMage/PlayerController",
            "KeyboardMage/Character/AbilitySystem"
        });
*/

        if (Target.Type == TargetType.Server)
        {
            PublicDefinitions.Add("UE_SERVER=1");
        }
    }
}