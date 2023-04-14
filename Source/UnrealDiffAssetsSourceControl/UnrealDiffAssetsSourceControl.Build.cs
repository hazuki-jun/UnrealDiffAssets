using UnrealBuildTool;

public class UnrealDiffAssetsSourceControl : ModuleRules
{
    public UnrealDiffAssetsSourceControl(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "Slate", "SlateCore",  "Projects", "Kismet", "ApplicationCore", "InputCore",
            "ToolMenus", "MainFrame", "DesktopPlatform", "Blutility", "AssetTools", "ContentBrowser", "EditorStyle", "EditorSubsystem",
            "GraphEditor", "UnrealEd", "BlueprintGraph", "DataTableEditor", "DeveloperSettings", "UnrealDiffAssetsEditor",
            "SourceControl"
        });
    }
}