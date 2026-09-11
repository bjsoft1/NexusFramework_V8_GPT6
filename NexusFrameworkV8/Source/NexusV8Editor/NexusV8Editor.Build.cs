using UnrealBuildTool;
public class NexusV8Editor : ModuleRules
{
    public NexusV8Editor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        bEnableExceptions = true;
        bUseUnity = false;
        PrivateIncludePaths.Add(System.IO.Path.Combine(ModuleDirectory, "Private"));
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "NexusV8Runtime", "NexusV8Authoring" });
        PrivateDependencyModuleNames.AddRange(new string[] { "NexusV8Compiler", "UnrealEd", "Landscape", "Slate", "SlateCore", "InputCore", "ToolMenus", "LevelEditor", "PropertyEditor", "DesktopPlatform", "Projects", "ApplicationCore", "AssetRegistry" });
    }
}
