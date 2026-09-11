using UnrealBuildTool;
public class NexusV8Compiler : ModuleRules
{
    public NexusV8Compiler(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
        bEnableExceptions = true;
        bUseUnity = false;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "NexusV8Runtime" });
        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "ProceduralMeshComponent", "RenderCore", "RHI" });
    }
}
