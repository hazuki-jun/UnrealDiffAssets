#include "UnrealDiffAssetsSourceControl.h"

#include "AssetViewUtils.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"
#include "ToolMenus.h"
#include "UnrealDiffWindowStyle.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffAssetsSourceControlModule"

void FUnrealDiffAssetsSourceControlModule::StartupModule()
{
	// BuildSourceControlMenu();
}

void FUnrealDiffAssetsSourceControlModule::ShutdownModule()
{
    
}

void FUnrealDiffAssetsSourceControlModule::BuildSourceControlMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->AddSection("UnrealDiffAssetsOptions", LOCTEXT("UnrealDiffAssetsOptionsLabel", "Unreal Differ Plugin"));

	Section.AddSubMenu(
		"UnrealDiffSCSubMenu",
		LOCTEXT("UnrealDiffSCSubMenuLabel", "Source Control"),
		LOCTEXT("UnrealDiffSCSubMenuToolTip", "Source control actions."),
		FNewToolMenuDelegate::CreateRaw(this, &FUnrealDiffAssetsSourceControlModule::FillSourceControlSubMenu),
		FUIAction(FExecuteAction()),
		EUserInterfaceActionType::Button,
		false,
		FSlateIcon(FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.StatusIcon.On"))
	);
}

void FUnrealDiffAssetsSourceControlModule::FillSourceControlSubMenu(UToolMenu* Menu)
{
	FToolMenuSection& Section = Menu->AddSection("UnrealDiffAssetsSCOptions", LOCTEXT("UnrealDiffAssetsSCOptionsLabel", "Source Control"));
	// Section.AddSeparator(NAME_None);
	Section.AddMenuEntry(
		"UnrealDiffSCCUpdate",
		LOCTEXT("UnrealDiffSCCUpdate_Label", "Update"),
		LOCTEXT("UnrealDiffSCCUpdate_Tooltip", "Update"),
		FSlateIcon(FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.Actions.Refresh")),
		FUIAction(FExecuteAction::CreateRaw(this, &FUnrealDiffAssetsSourceControlModule::OnMenuUpdateClicked))
	);
}

void FUnrealDiffAssetsSourceControlModule::OnMenuUpdateClicked()
{
	TArray<FString> PackageNames;
	// GetSelectedPackageNames(PackageNames);
	AssetViewUtils::SyncPackagesFromSourceControl(PackageNames);

	// ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>(), SourceControlHelpers::PackageFilenames(PackageNames), EConcurrency::Asynchronous);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsSourceControlModule, UnrealDiffAssetsSourceControl)
