// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscExtension.h"

#include "BlueprintEditorModule.h"
#include "FUDATextPropertyExtension.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "UMGEditorModule.h"
#include "UnrealDiffWindowStyle.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "UObject/UnrealTypePrivate.h"

#define LOCTEXT_NAMESPACE "FMiscExtensionModule"

static void RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensionButtons)
{
	// local copy for capturing in handlers below
	TSharedPtr<IPropertyHandle> PropertyHandle = Args.PropertyHandle;
	if (!PropertyHandle.IsValid())
	{
		return;
	}

	if (!CastField<FTextProperty>(PropertyHandle->GetProperty()))
	{
		return;
	}
	
	static FSlateIcon CreateKeyIcon(FAppStyle::Get().GetStyleSetName(), "Plus");

	TWeakPtr<IDetailTreeNode> OwnerTreeNode = Args.OwnerTreeNode;

	FPropertyRowExtensionButton& CreateKey = OutExtensionButtons.AddDefaulted_GetRef();
	CreateKey.Icon = CreateKeyIcon;
	CreateKey.Label = NSLOCTEXT("PropertyEditor", "AddSourceString", "Add Source String");
	CreateKey.ToolTip = NSLOCTEXT("PropertyEditor", "AddSourceStringToolTip", "Add a source string to string table");
	CreateKey.UIAction = FUIAction();
}

void FMiscExtensionModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FMiscExtensionModule::OnEngineInit);
}

TSharedRef<FExtender> FMiscExtensionModule::GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects)
{
	if (ContextSensitiveObjects.Num() <= 0 || !ContextSensitiveObjects[0]->IsA(UWidgetBlueprint::StaticClass()))
	{
		return MakeShared<FExtender>();
	}

	auto WidgetBlueprint = Cast<UWidgetBlueprint>(ContextSensitiveObjects[0]);
	if (!WidgetBlueprint)
	{
		return MakeShared<FExtender>();
	}
	
	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	TSharedPtr<class FUICommandList> PluginCommands = MakeShareable(new FUICommandList);
	ToolbarExtender->AddToolBarExtension("Debugging", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FMiscExtensionModule::FillToolbar));

	return ToolbarExtender;
}

void FMiscExtensionModule::FillToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.BeginSection(TEXT("MiscExtension"));
	{
		ToolbarBuilder.AddComboButton(
			FUIAction(),
			FOnGetContent::CreateRaw(this, &FMiscExtensionModule::GenerateExtensionMenuContent),
			LOCTEXT("MiscExtensionMenu_Label", "Extension"),
			LOCTEXT("MMiscExtensionMenu_ToolTip", "UnrealDiffAssets Misc Extension"),
			FSlateIcon(FUnrealDiffWindowStyle::GetMyPluginIcon()),
			false);
	}
	ToolbarBuilder.EndSection();
}

TSharedRef<SWidget> FMiscExtensionModule::GenerateExtensionMenuContent()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.AddMenuEntry(
				LOCTEXT("MiscExtensionMenuActions_AddSourceString", "Add Source String"),
				LOCTEXT("DetailSingleItemRowActions_CopyTooltip", "Add a source string to string table"),
				FUnrealDiffWindowStyle::GetAppSlateIcon("Plus"),
				FUIAction());

	return MenuBuilder.MakeWidget();
}

void FMiscExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FMiscExtensionModule::OnEngineInit()
{
	TextPropertyExtensionPtr = MakeShareable(new FUDATextPropertyExtension);
	TextPropertyExtensionPtr->Initialize();
	
	// FUDATextPropertyExtension::Initialize();
	
	// FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	// {
	// 	auto& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	// 	auto& ExtenderDelegates = BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates();
	// 	ExtenderDelegates.Add(FAssetEditorExtender::CreateRaw(this, &FMiscExtensionModule::GetBlueprintToolExtender));
	// });
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscExtensionModule, MiscExtension)
