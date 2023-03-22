using UnrealBuildTool;

public class UnrealDiffAssetsEditor : ModuleRules
{
	public UnrealDiffAssetsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore",  "Projects", "Kismet",
			"ToolMenus", "MainFrame", "DesktopPlatform", "Blutility", "AssetTools", "ContentBrowser", "EditorStyle",
			"GraphEditor", "UnrealEd", "BlueprintGraph"
		});
	}
}