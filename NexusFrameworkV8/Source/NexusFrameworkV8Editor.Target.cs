using UnrealBuildTool;
using System.Collections.Generic;
public class NexusFrameworkV8EditorTarget : TargetRules
{
    public NexusFrameworkV8EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("NexusV8Runtime");
    }
}
