#include "UnrealDiffAssetsEditor.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "EditorUtilityLibrary.h"
#include "IDesktopPlatform.h"
#include "ObjectTools.h"
#include "ToolMenus.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffAssetsEditorModule"

void FUnrealDiffAssetsEditorModule::StartupModule()
{
	const auto Path = IPluginManager::Get().FindPlugin(TEXT("UnrealDiffAssets"))->GetBaseDir() / "Content/";
	if (!FPaths::DirectoryExists(Path))
	{
		return;
	}
	
	TArray<FString> AllProtoCodeFiles;
	
	auto SearchSuffixFiles = [=,&AllProtoCodeFiles](const FString& pSuffix)
	{
		IFileManager::Get().FindFilesRecursive(AllProtoCodeFiles, *Path, *pSuffix, true, false, false);
	};
	
	for (const FString& Suffix : {TEXT("*.uasset")})
	{
		SearchSuffixFiles(Suffix);
	}
	
	for (const auto& File : AllProtoCodeFiles)
	{
		if (FPaths::FileExists(File))
		{
			IFileManager::Get().Delete(*File, true);
		}
	}
	
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->AddSection("UnrealDiffAssetsSection");
	
	FToolMenuEntry Entry = FToolMenuEntry::InitMenuEntry(
		TEXT("DiffAsset"),
		LOCTEXT("DiffAssetLable","Diff Asset"),
		LOCTEXT("DiffAsset_ToolTip","Diff Asset"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnDiffAssetMenuClicked(); })));
	
	Section.AddEntry(Entry);

	FSlateApplication::Get().OnWindowBeingDestroyed().AddLambda([this](const SWindow& Window)
	{
		if (Window.ToString().Find(TEXT("Blueprint Diff")) != INDEX_NONE)
		{
			// FTimerHandle TimerHandle;
			// FTimerDelegate TimerDelegate;
			// TimerDelegate.BindLambda([this]()
			// {
			// 	DeleteUAssets();
			// });
					
			// GEditor->GetTimerManager()->SetTimer(TimerHandle, TimerDelegate, 3.0f, false, 3.f);
		}
	});
}

void FUnrealDiffAssetsEditorModule::ShutdownModule()
{
    
}

void FUnrealDiffAssetsEditorModule::OnDiffAssetMenuClicked()
{
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
			UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *OutFiles[0], LOAD_None);
			if (AssetPkg)
			{
				AssetB = AssetPkg->FindAssetInPackage();
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

void FUnrealDiffAssetsEditorModule::DeleteUAssets()
{
	UObject* AssetB = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetBPath);
	if (AssetB)
	{
		ObjectTools::DeleteObjectsUnchecked({AssetB});
	}
	AssetBPath = "";
}

void FUnrealDiffAssetsEditorModule::ExecuteDiffAssets(UObject* AssetA, UObject* AssetB)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	FRevisionInfo CurrentRevision; 
	CurrentRevision.Revision = TEXT("Local Blueprint");

	FRevisionInfo NewRevision; 
	NewRevision.Revision = TEXT("Remote Blueprint");
	
	AssetToolsModule.Get().DiffAssets(AssetA, AssetB, CurrentRevision, NewRevision);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsEditorModule, UnrealDiffAssetsEditor)