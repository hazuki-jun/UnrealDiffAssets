#pragma once

#include "CoreMinimal.h"

class FUDATextPropertyExtension : public TSharedFromThis<FUDATextPropertyExtension>
{
public:
	void Initialize();

	/** 添加部菜单按钮 */
	TSharedRef<FExtender> GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects);

	void FillToolbar(FToolBarBuilder& ToolbarBuilder, FName BlueprintName);

	TSharedRef<SWidget> OnGenerateToolbarMenu(FName BlueprintName);

	// 点击 AddSourceString
	void OnExtension_AddSourceString(FName BlueprintName);
	
	// 点击 SetStringTable 给此蓝图设置一个StringTable
	void OnExtension_SetStringTable(FName BlueprintName);
	
	void CreateSettingsWindow(FName BlueprintName);

	FReply OnUseAssetButtonClicked(FName InBlueprintName);

#if ENGINE_MAJOR_VERSION == 5 
	// 在 FText Property 后添加Apply按钮
	void RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensionButtons);
#else
	void RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& InArgs, FOnGenerateGlobalRowExtensionArgs::EWidgetPosition InWidgetPosition, TArray<TSharedRef<class SWidget>>& OutExtensions);
#endif

	FReply ApplySourceString_UE4(TSharedPtr<IPropertyHandle> PropertyHandle);
	
	void ApplySourceString(TSharedPtr<IPropertyHandle> PropertyHandle);
	
	FString IncrementStringTableSourceString(const class UStringTable* InStringTable, const FName& InBlueprintName, const FString& InSourceString);
	
	class UStringTable* GetStringTable(const FString& InStringTablePath);

	FName GetActiveWidgetBlueprintName();
	
	class UWidgetBlueprint* GetActiveWidgetBlueprint();
	
	TWeakPtr<class FWidgetBlueprintEditor> GetActiveWidgetBlueprintEditor();
	
protected:
	FText OnGetMyStringTableText() const;

	TArray<TSharedPtr<IPropertyHandle>> CachedPropertyHandle;
	
protected:
	FText MyStringTableText;
	FDelegateHandle OnGetGlobalRowExtensionHandle;
};




