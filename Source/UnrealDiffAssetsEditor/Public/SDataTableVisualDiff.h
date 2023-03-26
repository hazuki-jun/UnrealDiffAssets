// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Widgets/SCompoundWidget.h"

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

	TSharedRef<SWidget> BuildWidgetContent();
	
	TSharedRef<SWidget> BuildLayoutWidget(FText InTitle, UObject* AssetObject, bool bIsLocal);

	/**
	 * @brief 选中行/鼠标点击了行
	 * @param bIsLocal 
	 * @param RowId 
	 */
	void OnRowSelectionChanged(bool bIsLocal, FName RowId);

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
	 * @brief 导出DataTable指定行
	 * @param ValueStr [Out]
	 * @param DataTable 
	 * @param RowName 
	 */
	void ExportText(FString& ValueStr, UDataTable* DataTable, FName& RowName) const;

	/**
	 * @brief 拷贝行名到粘贴板
	 * @param RowName 
	 */
	void CopyRowName(const FName& RowName);

	//~ Begin Ctrl + C 
	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End Ctrl + C
	
	TSharedPtr<SWindow> GetParentWindow() const { return ParentWindow; }

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
public:
	//~ Cell
	void GetDataTableData(bool bIsLocal, TArray<FDataTableEditorColumnHeaderDataPtr> &OutAvailableColumns, TArray<FDataTableEditorRowListViewDataPtr> &OutAvailableRows);
	//~ Cell

	// 同步左右侧滚动
	void SyncVerticalScrollOffset(bool bIsLocal, float NewOffset);

	UObject* GetLocalAsset() const { return LocalAsset; }
	UObject* GetRemoteAsset() const { return RemoteAsset; }

	UObject* GetAssetObject(bool bIsLocal) const { return bIsLocal ? LocalAsset : RemoteAsset; }
	
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutLocal;
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutRemote;

	/**
	 * @brief 指定Row是否存在差异
	 * @param InLocalDataTable 
	 * @param InRemoteDataTable 
	 * @param RowName 
	 * @return 
	 */
	bool IsAnyDifferenceRowToTow(UDataTable* InLocalDataTable, UDataTable* InRemoteDataTable, FName& RowName);
	
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

	// Parent Window Window Size
	mutable FVector2D WindowSize;
};
