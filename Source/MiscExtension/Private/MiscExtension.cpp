// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscExtension.h"

#include "BlueprintEditorModule.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"

#define LOCTEXT_NAMESPACE "FMiscExtensionModule"

void FMiscExtensionModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		auto& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
		auto& ExtenderDelegates = BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates();
		ExtenderDelegates.Add(FAssetEditorExtender::CreateRaw(this, &FMiscExtensionModule::GetBlueprintToolExtender));	
	});
	
	
	// ExtenderDelegates.Add(FAssetEditorExtender::CreateLambda(
	// 	[&](const TSharedRef<FUICommandList>, const TArray<UObject*> ContextSensitiveObjects)
	// 	{
	// 		const auto TargetObject = ContextSensitiveObjects.Num() < 1 ? nullptr : Cast<UBlueprint>(ContextSensitiveObjects[0]);
	// 		return GetExtender(TargetObject);
	// 	}));
	
	// UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("AssetEditor.WidgetBlueprintEditor.ToolBar");
	// if (!Menu)
	// {
	// 	return;
	// }
	//
	// FToolMenuSection& Section = Menu->AddSection("MiscExtensionOptions");
	// FToolMenuEntry EntryDiffAsset = FToolMenuEntry::InitMenuEntry(
	// TEXT("MiscExtension"),
	// LOCTEXT("MiscExtensionLable","MiscExtension"),
	// LOCTEXT("MiscExtension_ToolTip","MiscExtension"),
	// FSlateIcon(FAppStyle::GetAppStyleSetName(), "Plus"),
	// FUIAction());
	// Section.AddEntry(EntryDiffAsset);
	
	// Section.AddSubMenu(
	// 	"MiscExtensionOptionsSubMenu",
	// 	LOCTEXT("MiscExtensionOptionsSubMenuLabel", "Source Control"),
	// 	LOCTEXT("MiscExtensionOptionsSubMenuToolTip", "Source control actions."),
	// 	FNewToolMenuDelegate::CreateRaw(this, &FUnrealDiffAssetsSourceControlModule::FillSourceControlSubMenu),
	// 	FUIAction(FExecuteAction()),
	// 	EUserInterfaceActionType::Button,
	// 	false,
	// 	FSlateIcon(FAppStyle::GetAppStyleSetName(), "Plus")
	// );
	
	// TSharedRef<FExtender> ToolbarExtender(new FExtender());
	// UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	//
	// FToolMenuSection& Section = InMenu->AddSection("WidgetTools");
	// Section.InsertPosition = FToolMenuInsert("SourceControl", EToolMenuInsertType::After);
	//
	// Section.AddEntry(FToolMenuEntry::InitToolBarButton(
	// 	"OpenWidgetReflector",
	// 	FUIAction(
	// 		FExecuteAction::CreateLambda([=] { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("WidgetReflector")); }),
	// 		FCanExecuteAction()
	// 	)
	// 	, LOCTEXT("OpenWidgetReflector", "Widget Reflector")
	// 	, LOCTEXT("OpenWidgetReflectorToolTip", "Opens the Widget Reflector, a handy tool for diagnosing problems with live widgets.")
	// 	, FSlateIcon(FCoreStyle::Get().GetStyleSetName(), "WidgetReflector.Icon")
	// ));

	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// TSharedRef<FExtender> ToolbarExtender(new FExtender());
	// ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FMiscExtensionModule::FillToolbar));
}

TSharedRef<FExtender> FMiscExtensionModule::GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects)
{
	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	TSharedPtr<class FUICommandList> PluginCommands = MakeShareable(new FUICommandList);
	ToolbarExtender->AddToolBarExtension("Debugging", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FMiscExtensionModule::FillToolbar));

	return ToolbarExtender;
}

void FMiscExtensionModule::FillToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.BeginSection(TEXT("MiscExtension"));
	{
		ToolbarBuilder.AddToolBarButton(
			FUIAction(),
			FName(TEXT("Misc Extension")),
			LOCTEXT("MiscExtension_label", "Bridge"),
			LOCTEXT("MiscExtension_ToolTip", "Misc Extension"),
			FSlateIcon(),
			EUserInterfaceActionType::Button,
			FName(TEXT("MiscExtension"))
		);
	}
	ToolbarBuilder.EndSection();
}


void FMiscExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscExtensionModule, MiscExtension)