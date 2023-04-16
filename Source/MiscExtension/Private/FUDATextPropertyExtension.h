#pragma once

#include "CoreMinimal.h"

class FUDATextPropertyExtension : public TSharedFromThis<FUDATextPropertyExtension>
{
public:
	void Initialize();

	/** 添加部菜单按钮 */
	TSharedRef<FExtender> GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects);

	void FillToolbar(FToolBarBuilder& ToolbarBuilder);

	TSharedRef<SWidget> OnGenerateToolbarMenu();

	// 点击 AddSourceString
	void OnExtension_AddSourceString();
	
	// 点击 SetStringTable 给此蓝图设置一个StringTable
	void OnExtension_SetStringTable();
	
	void CreateSettingsWindow();

	FReply OnUseAssetButtonClicked();
	
	// 在 FText Property 后添加Apply按钮
	void RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensionButtons);
	
	void ApplySourceString();
	
	FString IncrementStringTableSourceString(const class UStringTable* InStringTable);
	
	class UStringTable* GetStringTable(const FString& InStringTablePath);

	void SetEditorObject(UObject* InEditorObject);

	FName GetActiveWidgetBlueprintName();
	
	TWeakPtr<class FWidgetBlueprintEditor> GetActiveWidgetBlueprintEditor();

protected:
	FText OnGetMyStringTableText() const;
	
protected:
	FText MyStringTableText;
	
	TWeakObjectPtr<UObject> EditorObject;
	TWeakPtr<IPropertyHandle> PropertyHandle;
	FDelegateHandle OnGetGlobalRowExtensionHandle;
};




