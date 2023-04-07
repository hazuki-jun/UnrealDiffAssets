// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "Widgets/SCompoundWidget.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

namespace EDataTableVisualDiff
{
	enum RowViewOption
	{
		Normal = 0x00000001,
		Modify = 0x00000002,
		Added = 0x00000004,
		Removed = 0x00000008,
		
		Max = Normal + Modify + Added + Removed
	};
}

ENUM_CLASS_FLAGS(EDataTableVisualDiff::RowViewOption);

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SDataTableVisualDiff : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDataTableVisualDiff){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void InitSettings();
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	TSharedRef<SWidget> BuildWidgetContent();

	float GetRowDetailViewSplitterValue() const;

	TSharedRef<SWidget> MakeToolbar();
	
	TSharedRef<SWidget> BuildLayoutWidget(FText InTitle, bool bIsLocal);

	TSharedRef<SWidget> BuildRowDetailView(bool bIsLocal);

	TSharedPtr<SWindow> GetParentWindow() const { return ParentWindow; }

	TSharedPtr<FStructOnScope> GetStructure();

	//~ Begin Toolbar
	
	// Toolbar Action
	// Next
	void ToolbarAction_HighlightNextDifference();
	// Prev
	void ToolbarAction_HighlightPrevDifference();
	// Diff
	void ToolbarAction_Diff();
	bool ToolbarAction_CanDiff();
	// Merge
	void ToolbarAction_Merge();
	//~ Toolbar Action

	// ViewOption Content
	TSharedRef<SWidget> GetShowViewOptionContent();

	/** Called when show only view option is clicked */
	void OnShowOnlyViewOptionClicked(EDataTableVisualDiff::RowViewOption InViewOption);
	
	bool HasRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption) const;
	void ClearRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption);
	void SetRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption);
	void SetRowViewOptionTo(EDataTableVisualDiff::RowViewOption InViewOption);
	void ReverseRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption);
	void ModifyConfig();
	
	// Modified Check State
	bool IsShowOnlyRowViewOptionChecked(EDataTableVisualDiff::RowViewOption InViewOption) const;
	//~ End Toolbar
	
	/**
	 * @brief 选中行/鼠标点击了行
	 * @param bIsLocal 
	 * @param RowId
	 * @param RowNumber 
	 */
	void OnRowSelectionChanged(bool bIsLocal, FName RowId, int32 RowNumber);

	/**
	 * @brief 拷贝指定行到粘贴板
	 * @param bIsLocal 
	 * @param RowName 
	 */
	void CopyRow(bool bIsLocal, const FName& RowName);

	/**
	 * @brief 拷贝选中的行到粘贴板
	 */
	void CopySelectedRow();
	
	/**
	 * @brief 拷贝行名到粘贴板
	 * @param RowName 
	 */
	void CopyRowName(const FName& RowName);

	/**
	 * @brief 复制右侧行到左侧, 左侧行不存在是会添加一行
	 * @param RowName 
	 */
	void MergeAction_MergeRow(const FName& RowName);
	
	/**
	 * @brief 删除指定行
	 * @param RowName 
	 */
	void MergeAction_DeleteRow(FName RowName);
	
	void ShowDifference_RowToRow(const FName& RowName, int32 InSelectedRowNumber);
	
	void RefreshLayout();
	
	//~ Begin Ctrl + C 
	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End Ctrl + C
	
	//~ Begin Row Detail View
	const uint8* GetPropertyData(const FProperty* InProperty);

	// DetailView -- 同步左右侧 展开/收起
	void SyncDetailViewAction_Expanded(bool bIsLocal, bool bIsExpanded, int32 NodeIndex);

	// DetailView -- 同步左右侧滚动
	void SyncDetailViewAction_VerticalScrollOffset(bool bIsLocal, float ScrollOffset);

	// DetailView -- 粘贴选中Property到左侧
	void DetailViewAction_MergeProperty(int32 NodeIndex, const FString& PropertyValueString, bool bRegenerate = false);

	void CloseDetailView();
	
	//~ End Row Detail View
	
public:
	//~ Begin DataTable Row
	// 同步左右侧滚动
	void SyncVerticalScrollOffset(bool bIsLocal, float NewOffset);
	// 获取行
	void GetDataTableData(bool bIsLocal, TArray<FDataTableEditorColumnHeaderDataPtr> &OutAvailableColumns, TArray<FDataTableEditorRowListViewDataPtr> &OutAvailableRows);
	FSlateColor GetHeaderColumnColorAndOpacity(bool InIsLocal, int32 Index) const;
	//~ End DataTable Row

	UObject* GetLocalAsset() const { return LocalAsset; }
	UDataTable* GetLocalDataTable() const { return Cast<UDataTable>(LocalAsset); }
	
	UObject* GetRemoteAsset() const { return RemoteAsset; }
	UDataTable* GetRemoteDataTable() const { return Cast<UDataTable>(RemoteAsset); }
	
	UObject* GetAssetObject(bool bIsLocal) const { return bIsLocal ? LocalAsset : RemoteAsset; }
	
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutLocal;
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutRemote;
	TSharedPtr<class SUnrealDiffDataTableRowDetailView> RowDetailViewLocal;
	TSharedPtr<class SUnrealDiffDataTableRowDetailView> RowDetailViewRemote;
	
protected:
	TSharedPtr<SWindow> ParentWindow;
	
	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;

	// Ctrl +C
	bool bPressedCtrl = false;

	// 标记选中的是本地还是对比的资源
	bool bIsLocalDataTableSelected = false;
	
	// 当前选中的行
	FName SelectedRowId;

	uint32 SelectedRowNumber = 0;

	// Parent Window Window Size
	mutable FVector2D WindowSize;

	EDataTableVisualDiff::RowViewOption RowViewOption = EDataTableVisualDiff::Max;
};

