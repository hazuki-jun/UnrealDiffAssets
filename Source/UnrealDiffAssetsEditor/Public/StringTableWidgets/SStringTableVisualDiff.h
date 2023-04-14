// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualDiffWidget.h"
#include "UnrealDiffStringTableEntry.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SStringTableVisualDiff : public SVisualDiffWidget
{
public:
	SLATE_BEGIN_ARGS(SStringTableVisualDiff){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	void InitSettings();

	/**
	 * @brief 选中指定行
	 * @param bIsLocal 
	 * @param RowKey
	 * @param RowNumber 
	 */
	void OnRowSelected(bool bIsLocal, FString RowKey, int32 RowNumber);

	/**
	 * @brief 同步左右2个选中状态
	 * @param bIsLocal 
	 * @param RowKey 
	 */
	void Sync_HighlightRow(bool bIsLocal, FString RowKey);
	void Sync_VerticalScrollOffset(bool bIsLocal, float ScrollOffset);
	
	//~ Begin SVisualDiffWidget Toolbar Action Interface
	virtual void ToolbarAction_HighlightNextDifference() override;
	virtual void ToolbarAction_HighlightPrevDifference() override;
	virtual bool IsDiffable() override;
	virtual void ToolbarAction_Merge() override;
	virtual void UpdateVisibleRows() override;
	//~ End SVisualDiffWidget Toolbar Action Interface

	// Begin SVisualDiffWidget ViewOption Interface
	virtual void RefreshLayout() override;
	virtual void ModifyConfig() override;
	// End SVisualDiffWidget ViewOption Interface
	
	class UStringTable* GetStringTable(bool bIsLocal);
	FStringTableConstRef GetStringTableRef(bool bIsLocal);

	/**
	 * @brief 获取指定行状态 Added/Removed/Modify/Normal
	 * @param bIsLocal 
	 * @param RowKey 
	 * @return [default] Normal
	 */
	int32 GetRowState(bool bIsLocal, const FString& RowKey);

	/**
	 * @brief 获取所有StringTable所有Entry
	 * @param bIsLocal 
	 * @param OutEntries 
	 */
	void GetStringTableData(bool bIsLocal, TArray<TSharedPtr<FCachedStringTableEntry>>& OutEntries);

	/**
	 * @brief Merge右侧表指定行到左侧
	 * @param RowKey 
	 */
	void PerformMerge(const FString& RowKey);
	
protected:
	TSharedPtr<class SUnrealDiffStringTableListView> StringTableListViewLocal;
	TSharedPtr<class SUnrealDiffStringTableListView> StringTableListViewRemote;
	
protected:
	TSharedPtr<SWindow> ParentWindow;
	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;

	FString SelectedRowKey;
	int32 SelectedRowNumber = -1;
};



