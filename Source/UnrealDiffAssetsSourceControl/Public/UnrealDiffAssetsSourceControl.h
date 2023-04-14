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
};
