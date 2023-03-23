using System.IO;
using UnrealBuildTool;

public class UnrealDiffAssetsEditor : ModuleRules
{
	public UnrealDiffAssetsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore",  "Projects", "Kismet", "ApplicationCore",
			"ToolMenus", "MainFrame", "DesktopPlatform", "Blutility", "AssetTools", "ContentBrowser", "EditorStyle",
			"GraphEditor", "UnrealEd", "BlueprintGraph"
		});
		
		
#if UE_5_0_OR_LATER
        
#else
		string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
		PrivateIncludePaths.AddRange(new string[] {Path.Combine(EngineDir, "Source/Editor/Kismet/Private")});
#endif

	}
}