#include "UnrealDiffAssetsSourceControl.h"

#include "AssetViewUtils.h"
#include "Dialogs.h"
#include "EditorUtilityLibrary.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "SourceControlHelpers.h"
#include "SourceControlOperations.h"
#include "SourceControlWindows.h"
#include "StringTable.h"
#include "ToolMenus.h"
#include "UnrealDiffAssetsEditor.h"
#include "UnrealDiffWindowStyle.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffAssetsSourceControlModule"

void FUnrealDiffAssetsSourceControlModule::StartupModule()
{
	BuildSourceControlMenu();

	 // SourceControlStateChanged_Handle = ISourceControlModule::Get().GetProvider().RegisterSourceControlStateChanged_Handle(FSourceControlStateChanged::FDelegate::CreateRaw(this, &FUnrealDiffAssetsSourceControlModule::OnSourceControlStateChanged));
}

void FUnrealDiffAssetsSourceControlModule::OnSourceControlStateChanged()
{
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() <= 0)
	{
		return;
	}

	if (SelectedAssets[0]->IsA(UDataTable::StaticClass()))
	{
		
	}
	else if (SelectedAssets[0]->IsA(UStringTable::StaticClass()))
	{
		
	}
}

void FUnrealDiffAssetsSourceControlModule::ShutdownModule()
{
	ISourceControlModule::Get().GetProvider().UnregisterSourceControlStateChanged_Handle(SourceControlStateChanged_Handle);
}

void FUnrealDiffAssetsSourceControlModule::BuildSourceControlMenu()
{
	if (!ISourceControlModule::Get().IsEnabled())
	{
		return;
	}
	
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
		LOCTEXT("UnrealDiffSCCUpdate_Label", "Sync"),
		LOCTEXT("UnrealDiffSCCUpdate_Tooltip", "Update this file in the workspace to the latest version"),
		FSlateIcon(FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.Actions.Sync")),
		FUIAction(FExecuteAction::CreateRaw(this, &FUnrealDiffAssetsSourceControlModule::OnMenuUpdateClicked))
	);
}

void FUnrealDiffAssetsSourceControlModule::OnMenuUpdateClicked()
{
	TArray<FAssetData> SelectAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	if (SelectAssets.Num() > 1 || SelectAssets.Num() <= 0)
	{
		return;
	}

	auto SelectObject =  SelectAssets[0].GetAsset();
	if (!SelectObject->IsA(UDataTable::StaticClass()) || SelectObject->IsA(UStringTable::StaticClass()))
	{
		// Not Supported
		return;
	}

	if (IsFileCurrent())
	{
		return;
	}
	
	FString DestFilePath = CopyFile(SelectAssets[0]);

	
	TArray<FString> PackagesName = { SelectAssets[0].PackageName.ToString() };
	TArray<FString> Files = SourceControlHelpers::PackageFilenames(PackagesName);
	RevertLocalFile(PackagesName, Files);
	AssetViewUtils::SyncPackagesFromSourceControl( PackagesName );
	EditConflict(DestFilePath);
}

void FUnrealDiffAssetsSourceControlModule::RevertLocalFile(const TArray<FString>& InPackageNames, const TArray<FString>& Files)
{
	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	// attempt to unload the packages we are about to revert
	TArray<UPackage*> LoadedPackages;
	for (TArray<FString>::TConstIterator PackageIter(InPackageNames); PackageIter; ++PackageIter)
	{
		UPackage* Package = FindPackage(NULL, **PackageIter);
		if (Package != NULL)
		{
			LoadedPackages.Add(Package);
		}
	}

	const TArray<FString> RevertPackageFilenames = SourceControlHelpers::PackageFilenames(Files);

	// Prepare the packages to be reverted...
	for (UPackage* Package : LoadedPackages)
	{
		// Detach the linkers of any loaded packages so that SCC can overwrite the files...
		if (!Package->IsFullyLoaded())
		{
			FlushAsyncLoading();
			Package->FullyLoad();
		}
		ResetLoaders(Package);
	}

	// Revert everything...
	SourceControlProvider.Execute(ISourceControlOperation::Create<FRevert>(), RevertPackageFilenames);

	// Reverting may have deleted some packages, so we need to unload those rather than re-load them...
	TArray<UPackage*> PackagesToUnload;
	LoadedPackages.RemoveAll([&](UPackage* InPackage) -> bool
	{
		const FString PackageExtension = InPackage->ContainsMap() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension();
		const FString PackageFilename = FPackageName::LongPackageNameToFilename(InPackage->GetName(), PackageExtension);
		if (!FPaths::FileExists(PackageFilename))
		{
			PackagesToUnload.Emplace(InPackage);
			return true; // remove package
		}
		return false; // keep package
	});

	// Hot-reload the new packages...
	UPackageTools::ReloadPackages(LoadedPackages);

	// Unload any deleted packages...
	UPackageTools::UnloadPackages(PackagesToUnload);

	// Re-cache the SCC state...
	SourceControlProvider.Execute(ISourceControlOperation::Create<FUpdateStatus>(), RevertPackageFilenames, EConcurrency::Asynchronous);
		
	
}

FString FUnrealDiffAssetsSourceControlModule::CopyFile(const FAssetData& AssetData)
{
	FString DestFilePath = (FPaths::Combine(*FPaths::DiffDir(), TEXT("CurrentAsset_Right_"))) + AssetData.GetAsset()->GetName();
	if (FPaths::FileExists(DestFilePath))
	{
		// ObjectTools::DeleteObjectsUnchecked({SelectObject});
		return FString();
	}
	else
	{
		TArray<FString> AllFiles = SourceControlHelpers::PackageFilenames({AssetData.PackageName.ToString()});
		FString SrcPath = AllFiles[0];
		DestFilePath = FPaths::ConvertRelativePathToFull(DestFilePath) + FString(".uasset");
		if (IFileManager::Get().Copy(*DestFilePath, *SrcPath) != COPY_OK)
		{
			// Copy failed
			return FString();
		}
	}

	return DestFilePath;
}

bool FUnrealDiffAssetsSourceControlModule::IsFileModified()
{
	TArray<FAssetData> SelectAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	if (SelectAssets.Num() > 1 || SelectAssets.Num() <= 0)
	{
		return false;
	}

	TArray<FString> AllFiles = SourceControlHelpers::PackageFilenames({SelectAssets[0].PackageName.ToString()});
	TArray< TSharedRef<ISourceControlState, ESPMode::ThreadSafe> > OutStates;
	ISourceControlModule::Get().GetProvider().GetState(AllFiles, OutStates, EStateCacheUsage::Use);
	for(TArray< TSharedRef<ISourceControlState, ESPMode::ThreadSafe> >::TConstIterator It(OutStates); It; It++)
	{
		TSharedRef<ISourceControlState, ESPMode::ThreadSafe> SourceControlState = *It;
		if (SourceControlState->IsModified())
		{
			return true;
		}
	}
	
	return false;	
}

bool FUnrealDiffAssetsSourceControlModule::IsFileCurrent()
{
	TArray<FAssetData> SelectAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	if (SelectAssets.Num() > 1 || SelectAssets.Num() <= 0)
	{
		return false;
	}

	TArray<FString> AllFiles = SourceControlHelpers::PackageFilenames({SelectAssets[0].PackageName.ToString()});
	TArray< TSharedRef<ISourceControlState, ESPMode::ThreadSafe> > OutStates;
	ISourceControlModule::Get().GetProvider().GetState(AllFiles, OutStates, EStateCacheUsage::Use);
	for(TArray< TSharedRef<ISourceControlState, ESPMode::ThreadSafe> >::TConstIterator It(OutStates); It; It++)
	{
		TSharedRef<ISourceControlState, ESPMode::ThreadSafe> SourceControlState = *It;
		if (SourceControlState->IsCurrent())
		{
			return true;
		}
	}
	
	return false;
}

void FUnrealDiffAssetsSourceControlModule::EditConflict(const FString& DestFilePath)
{
	TArray<FAssetData> SelectAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	if (SelectAssets.Num() > 1 || SelectAssets.Num() <= 0)
	{
		return;
	}

	const FText Title = LOCTEXT("EditConflictTitle", "Edit Changed"); 
	const FText Message = LOCTEXT("EditConflictWarning", "Edit Changed?");
	
	FSuppressableWarningDialog::FSetupInfo Info(Message, Title, FString());
	Info.ConfirmText = LOCTEXT("EditConflictYesButton", "Edit");
	Info.CancelText = LOCTEXT("EditConflictNoButton", "Cancel");

	FSuppressableWarningDialog EditConflictBlueprintDlg(Info);
	if (EditConflictBlueprintDlg.ShowModal() != FSuppressableWarningDialog::Confirm)
	{
		return;
	}

	UObject* RemoteAsset = nullptr;
	UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *DestFilePath, LOAD_None);
	if (AssetPkg)
	{
		RemoteAsset = AssetPkg->FindAssetInPackage();
	}

	if (!RemoteAsset)
	{
		return;
	}
	
	FUnrealDiffAssetsEditorModule::Get().Extern_ExecuteDiffAssets(SelectAssets[0].GetAsset(), RemoteAsset);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsSourceControlModule, UnrealDiffAssetsSourceControl)
