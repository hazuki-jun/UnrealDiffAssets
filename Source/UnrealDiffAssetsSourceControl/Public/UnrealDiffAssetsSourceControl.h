#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealDiffAssetsSourceControlModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
	
    virtual void ShutdownModule() override;

    void BuildSourceControlMenu();
    void FillSourceControlSubMenu(class UToolMenu* Menu);

    void OnMenuUpdateClicked();

	void RevertLocalFile(const TArray<FString>& InPackageNames, const TArray<FString>& Files);
	
	FString CopyFile(const FAssetData& AssetData);

	bool IsFileModified();

	bool IsFileCurrent();
	
	void EditConflict(const FString& DestFilePath);
	
	FDelegateHandle SourceControlStateChanged_Handle;
};
