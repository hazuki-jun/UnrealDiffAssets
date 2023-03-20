#include "UnrealDiffAssetsEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "EditorUtilityLibrary.h"
#include "IDesktopPlatform.h"
#include "ObjectTools.h"
#include "ToolMenus.h"
#include "Dialogs/Dialogs.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffAssetsEditorModule"

void FUnrealDiffAssetsEditorModule::StartupModule()
{
	DeleteUAssets();
	
	FSlateApplication::Get().OnWindowBeingDestroyed().AddLambda([this](const SWindow& Window)
	{
		if (Window.ToString().Find(TEXT("Blueprint Diff")) != INDEX_NONE)
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this]()
			{
				DeleteLoadedUAssets();
			});
					
			GEditor->GetTimerManager()->SetTimer(TimerHandle, TimerDelegate, 3.0f, false, 1.f);
		}
	});
	
	BuildDiffAssetsMenu();
}

void FUnrealDiffAssetsEditorModule::ShutdownModule()
{
}

void FUnrealDiffAssetsEditorModule::BuildDiffAssetsMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->AddSection("UnrealDiffAssetsSection");
	
#if ENGINE_MAJOR_VERSION == 5
	FToolMenuEntry Entry = FToolMenuEntry::InitMenuEntry(
		TEXT("DiffAsset"),
		LOCTEXT("DiffAssetLable","Diff Asset"),
		LOCTEXT("DiffAsset_ToolTip","Diff Asset"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnDiffAssetMenuClicked(); })));
	
	Section.AddEntry(Entry);
#else
	Section.AddMenuEntry(TEXT("DiffAsset"),
		LOCTEXT("DiffAssetLable","Diff Asset"),
		LOCTEXT("DiffAsset_ToolTip","Diff Asset"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnDiffAssetMenuClicked(); })));
#endif
}

void FUnrealDiffAssetsEditorModule::OnDiffAssetMenuClicked()
{
	if (!IsSupported())
	{
		return;
	}
	
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return;
	}
	
	FString Filter = TEXT("uasset file|*.uasset");
	TArray<FString> OutFiles;
	UObject* AssetB = nullptr;
	for (;;)
	{
		if (DesktopPlatform->OpenFileDialog(nullptr, TEXT("Choose Another File"), TEXT(""), TEXT(""), Filter, EFileDialogFlags::None, OutFiles))
		{
			if (OutFiles.Num() > 0)
			{
				FString DestFilePath = (FPaths::Combine(*FPaths::DiffDir(), TEXT("MergeTool-Left")));
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
				
				AssetBPath = DestFilePath;
				UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *DestFilePath, LOAD_None);
				if (AssetPkg)
				{
					AssetB = AssetPkg->FindAssetInPackage();
				}
			}
		}
		break;
	}
	
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() > 0)
	{
		if (AssetB)
		{
			ExecuteDiffAssets(AssetB, SelectedAssets[0]);
		}
	}
	
	// DeleteUAssets();
	// //
	// IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	// if (!DesktopPlatform)
	// {
	// 	return;
	// }
	// FString Filter = TEXT("uasset file|*.uasset");
	// FString Diff_AssetBName;
	// FString AssetBName;
	// TArray<FString> OutFiles;
	// for (;;)
	// {
	// 	if (DesktopPlatform->OpenFileDialog(nullptr, TEXT("Choose Another File"), TEXT(""), TEXT(""), Filter, EFileDialogFlags::None, OutFiles))
	// 	{
	// 		if (OutFiles.Num() > 0)
	// 		{
	// 			Diff_AssetBName = AssetBName = OutFiles[0];
	// 				
	// 			int32 Pos;
	// 			if (AssetBName.FindLastChar('/', Pos))
	// 			{
	// 				AssetBName.RemoveAt(0, Pos + 1);
	// 			}
	// 			Diff_AssetBName = "Diff_" + AssetBName;
	//
	// 			UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *OutFiles[0], LOAD_None);
	// 			auto Asset = AssetPkg->FindAssetInPackage();
	// 			const FString FileName = IPluginManager::Get().FindPlugin(TEXT("UnrealDiffAssets"))->GetBaseDir() / "Content"/ Diff_AssetBName;
	// 			IFileManager::Get().Copy(*FileName, *OutFiles[0]);
	// 		}
	// 	}
	// 	break;
	// }
	//
	// TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	// if (SelectedAssets.Num() > 0)
	// {
	// 	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	// 	FString Suffix;
	// 	AssetBName.Split(".", &AssetBName, &Suffix);
	// 	AssetBPath = "'/UnrealDiffAssets" / FString("Diff_") + AssetBName + "." + AssetBName + "'";
	// 	UObject* AssetB = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetBPath);
	// 	if (AssetB)
	// 	{
	// 		ExecuteDiffAssets(AssetB, SelectedAssets[0]);
	// 	}
	// }
}

bool FUnrealDiffAssetsEditorModule::IsSupported()
{
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() <= 0 || !SelectedAssets[0])
	{
		return false;
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	
	TWeakPtr<IAssetTypeActions> Actions = AssetToolsModule.Get().GetAssetTypeActionsForClass( SelectedAssets[0]->GetClass() );

	TSet<FName> NotSupportClasses = { TEXT("DataTable") };
		
	auto SupportClass = Actions.Pin()->GetSupportedClass();
	if (SupportClass && NotSupportClasses.Contains(SupportClass->GetFName()))
	{
		FText NotSupportWarning = LOCTEXT("NotSupportWarningMessage", "This asset currently not supported");
		FSuppressableWarningDialog::FSetupInfo Info( NotSupportWarning, LOCTEXT("NotSupport_Message", "Not Support"), "NotSupport_Warning" );
		Info.ConfirmText = LOCTEXT( "NotSupport_Yes", "Ok");
		Info.CancelText = LOCTEXT( "NotSupport_No", "Cancel");	
		FSuppressableWarningDialog RemoveLevelWarning( Info );
		auto Result = RemoveLevelWarning.ShowModal();
		return false;
	}

	return true;
}

void FUnrealDiffAssetsEditorModule::DeleteLoadedUAssets()
{
	UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *AssetBPath, LOAD_None);
	if (AssetPkg)
	{
		UObject* AssetB = AssetPkg->FindAssetInPackage();
		if (AssetB)
		{
			ObjectTools::DeleteObjectsUnchecked({AssetB});
		}
	}
	AssetBPath = "";
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

void FUnrealDiffAssetsEditorModule::ExecuteDiffAssets(UObject* AssetA, UObject* AssetB)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	FRevisionInfo CurrentRevision; 
	CurrentRevision.Revision = TEXT("Local Blueprint");

	FRevisionInfo NewRevision; 
	NewRevision.Revision = TEXT("Remote Blueprint");

	if (AssetA->GetClass() == AssetB->GetClass())
	{
		if (!IsSupported())
		{
			return;
		}
	}
	else
	{
		DeleteLoadedUAssets();
		return;
	}
	
	AssetToolsModule.Get().DiffAssets(AssetA, AssetB, NewRevision, CurrentRevision);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsEditorModule, UnrealDiffAssetsEditor)
