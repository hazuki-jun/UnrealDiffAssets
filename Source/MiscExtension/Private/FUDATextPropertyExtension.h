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
	
	// 在 FText Property 后添加Apply按钮
	void RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensionButtons);
	
	void ApplySourceString();
	
	FString IncrementStringTableSourceString(const class UStringTable* InStringTable, const FName& InBlueprintName, const FString& InSourceString);
	
	class UStringTable* GetStringTable(const FString& InStringTablePath);

	void SetEditorObject(UObject* InEditorObject);

	FName GetActiveWidgetBlueprintName();
	
	class UWidgetBlueprint* GetActiveWidgetBlueprint();
	
	TWeakPtr<class FWidgetBlueprintEditor> GetActiveWidgetBlueprintEditor();
	
protected:
	FText OnGetMyStringTableText() const;
	
protected:
	FText MyStringTableText;
	
	TWeakObjectPtr<UObject> EditorObject;
	TWeakPtr<IPropertyHandle> PropertyHandle;
	FDelegateHandle OnGetGlobalRowExtensionHandle;
};




