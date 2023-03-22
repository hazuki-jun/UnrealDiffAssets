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

	bool IsSupported();

	void OnDiffWindowClosed();
	
    void DeleteLoadedUAssets();

	void DeleteUAssets();
	
    void ExecuteDiffAssets(UObject* AssetA, UObject* AssetB);

	void PerformDiffAction(UObject* AssetA, UObject* AssetB);

    FString RemoteAssetPath;
};
