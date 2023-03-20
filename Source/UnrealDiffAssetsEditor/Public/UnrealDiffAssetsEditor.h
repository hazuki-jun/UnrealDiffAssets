#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnrealDiffAssetsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	void BuildDiffAssetsMenu();
	
    void OnDiffAssetMenuClicked();

    void DeleteUAssets();
    
    void ExecuteDiffAssets(UObject* AssetA, UObject* AssetB);


    FString AssetBPath;
};
