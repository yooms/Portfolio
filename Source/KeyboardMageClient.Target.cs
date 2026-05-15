// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class KeyboardMageClientTarget : TargetRules
{
    public KeyboardMageClientTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Client;

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange(new string[]
        {
            "KeyboardMage",
            "KeyboardMageClient"
        });
    }
}