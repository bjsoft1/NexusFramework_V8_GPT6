using UnrealBuildTool;
using System.Collections.Generic;
public class NexusFrameworkV8Target : TargetRules
{
    public NexusFrameworkV8Target(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("NexusV8Runtime");
    }
}
