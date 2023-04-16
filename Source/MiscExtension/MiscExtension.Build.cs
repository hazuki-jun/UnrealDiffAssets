// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MiscExtension : ModuleRules
{
	public MiscExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "Slate", "SlateCore",  "Projects", "Kismet", "ApplicationCore", "InputCore",
			"ToolMenus", "MainFrame", "DesktopPlatform", "Blutility", "AssetTools", "ContentBrowser", "EditorStyle", "EditorSubsystem",
			"GraphEditor", "UnrealEd", "BlueprintGraph", "DataTableEditor", "DeveloperSettings", "UMGEditor", "UMGEditor", "Kismet", "UnrealDiffAssetsEditor"
		});
		
		string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
		
		PrivateIncludePaths.AddRange(new string[] {Path.Combine(EngineDir, "Source/Editor/UMGEditor/Private")});
		// PrivateIncludePaths.AddRange(new string[] {Path.Combine(EngineDir, "Source/Editor/Kismet/Private")});
		
		// DynamicallyLoadedModuleNames.AddRange(
		// 	new[]
		// 	{
		// 		"Kismet",
		// 		"MainFrame",
		// 		"AnimationBlueprintEditor",
		// 	}
		// );
	}
}
