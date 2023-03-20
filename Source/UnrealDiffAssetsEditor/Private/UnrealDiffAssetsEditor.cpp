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
	
	AssetToolsModule.Get().DiffAssets(AssetA, AssetB, NewRevision, CurrentRevision);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUnrealDiffAssetsEditorModule, UnrealDiffAssetsEditor)