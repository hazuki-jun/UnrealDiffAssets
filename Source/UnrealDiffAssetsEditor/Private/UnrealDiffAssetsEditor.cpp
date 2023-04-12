#include "UnrealDiffAssetsEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "DiffClassCollectionSubsystem.h"
#include "EditorUtilityLibrary.h"
#include "IDesktopPlatform.h"
#include "ObjectTools.h"
#include "SBlueprintDiffWindow.h"
#include "BlueprintWidgets/SBlueprintVisualDiff.h"
#include "ToolMenus.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
#include "Dialogs/Dialogs.h"
#include "Framework/Commands/GenericCommands.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffAssetsEditorModule"

void FUnrealDiffAssetsEditorModule::StartupModule()
{
	FUnrealDiffWindowStyle::Initialize();
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	
	DeleteUAssets();
	BuildDiffAssetsMenu();
	
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.BindRaw(this, &FUnrealDiffAssetsEditorModule::OnDiffWindowClosed);
}

void FUnrealDiffAssetsEditorModule::ShutdownModule()
{
}

void FUnrealDiffAssetsEditorModule::BuildDiffAssetsMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->AddSection("UnrealDiffAssetsOptions", LOCTEXT("UnrealDiffAssetsOptionsLabel", "Unreal Differ Plugin"));
#if ENGINE_MAJOR_VERSION == 5
	FToolMenuEntry EntryDiffAsset = FToolMenuEntry::InitMenuEntry(
		TEXT("Diff"),
		LOCTEXT("DiffAssetDiffLable","Diff"),
		LOCTEXT("DiffAssetDiff_ToolTip","Diff"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnOptionDiffClicked(); })));
	
	Section.AddEntry(EntryDiffAsset);
#else
	Section.AddMenuEntry(TEXT("DiffAssetDiff"),
		LOCTEXT("DiffAssetDiffLable","Diff"),
		LOCTEXT("DiffAssetDiff_ToolTip","Diff"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnDiffAssetMenuClicked(); })));
#endif

#if ENGINE_MAJOR_VERSION == 5
	FToolMenuEntry EntryUpdate = FToolMenuEntry::InitMenuEntry(
		TEXT("Update"),
		LOCTEXT("DiffAssetUpdateLable","Update"),
		LOCTEXT("DiffAssetUpdate_ToolTip","Update"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Refresh"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnOptionUpdateClicked(); })));
	
	Section.AddEntry(EntryUpdate);
#else
	Section.AddMenuEntry(TEXT("Update"),
		LOCTEXT("DiffAssetUpdateLable","Update"),
		LOCTEXT("DiffAssetUpdate_ToolTip","Update"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.Refresh"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnOptionUpdateClicked(); })));
#endif
}

void FUnrealDiffAssetsEditorModule::OnOptionDiffClicked()
{
	static int32 DiffCount = 0;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return;
	}
	
	FString Filter = TEXT("uasset file|*.uasset");
	TArray<FString> OutFiles;
	UObject* RemoteAsset = nullptr;
	for (;;)
	{
		if (DesktopPlatform->OpenFileDialog(nullptr, TEXT("Choose Another File"), TEXT(""), TEXT(""), Filter, EFileDialogFlags::None, OutFiles))
		{
			if (OutFiles.Num() > 0)
			{
				FString DestFilePath = (FPaths::Combine(*FPaths::DiffDir(), TEXT("AssetToDiff_Right_"))) + FString::FromInt(DiffCount++);
				const FString& AssetExt = FPackageName::GetAssetPackageExtension();
				if (!DestFilePath.EndsWith(AssetExt))
				{
					DestFilePath += AssetExt;
				}
				
				if (IFileManager::Get().Copy(*DestFilePath, *OutFiles[0]) != COPY_OK)
				{
					UE_LOG(LogTemp, Error, TEXT("Copy failed: %s"), *DestFilePath)
					return;
				}
				
				RemoteAssetPath = DestFilePath;
				UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *DestFilePath, LOAD_None);
				if (AssetPkg)
				{
					RemoteAsset = AssetPkg->FindAssetInPackage();
				}
			}
		}
		break;
	}
	
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() > 0)
	{
		if (RemoteAsset)
		{
			ExecuteDiffAssets(SelectedAssets[0], RemoteAsset);
		}
	}
}

void FUnrealDiffAssetsEditorModule::OnOptionUpdateClicked()
{
	
}

bool FUnrealDiffAssetsEditorModule::IsSupported()
{
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() <= 0 || !SelectedAssets[0])
	{
		return false;
	}

	if (!GDiffClassCollectionSubsystem.IsSupported(SelectedAssets[0]))
	{
		return false;
	}

	return true;
}

void FUnrealDiffAssetsEditorModule::OnDiffWindowClosed()
{
	// DeleteLoadedUAssets();
}

void FUnrealDiffAssetsEditorModule::DeleteLoadedUAssets()
{
	if (RemoteAssetPath.IsEmpty())
	{
		return;
	}
	
	UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *RemoteAssetPath, LOAD_None);
	if (AssetPkg)
	{
		UObject* RemoteAsset = AssetPkg->FindAssetInPackage();
		if (RemoteAsset)
		{
			ObjectTools::DeleteObjectsUnchecked({RemoteAsset});
			RemoteAssetPath = "";
		}
	}
}

void FUnrealDiffAssetsEditorModule::DeleteUAssets()
{
	const auto Path = FPaths::Combine(*FPaths::DiffDir());
	if (!FPaths::DirectoryExists(Path))
	{
		return;
	}
	
	TArray<FString> Files;
	
	auto SearchSuffixFiles = [=,&Files](const FString& pSuffix)
	{
		IFileManager::Get().FindFilesRecursive(Files, *Path, *pSuffix, true, false, false);
	};
	
	for (const FString& Suffix : {TEXT("*.uasset")})
	{
		SearchSuffixFiles(Suffix);
	}
	
	for (const auto& File : Files)
	{
		if (FPaths::FileExists(File))
		{
			IFileManager::Get().Delete(*File, true);
		}
	}
}

void FUnrealDiffAssetsEditorModule::ExecuteDiffAssets(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (LocalAsset->GetClass() == RemoteAsset->GetClass())
	{
		if (!IsSupported())
		{
			FRevisionInfo RevisionInfo;
			RevisionInfo.Revision = TEXT("");
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
			AssetToolsModule.Get().DiffAssets(LocalAsset, RemoteAsset, RevisionInfo, RevisionInfo);
			return;
		}
	}
	else
	{
		FText NotSupportWarning = LOCTEXT("NotSupportWarningMessage", "类型不一致");
		FSuppressableWarningDialog::FSetupInfo Info( NotSupportWarning, LOCTEXT("NotSupport_Message", "Not Support"), "NotSupport_Warning" );
		Info.ConfirmText = LOCTEXT( "NotSupport_Yes", "Ok");
		Info.CancelText = LOCTEXT( "NotSupport_No", "Cancel");	
		FSuppressableWarningDialog RemoveLevelWarning( Info );
		auto Result = RemoveLevelWarning.ShowModal();
		return;
	}

	PerformDiffAction(LocalAsset, RemoteAsset);
}

void FUnrealDiffAssetsEditorModule::PerformDiffAction(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	auto BlueprintDiffWindow = SBlueprintDiffWindow::CreateWindow(LocalAsset, RemoteAsset);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsEditorModule, UnrealDiffAssetsEditor)
