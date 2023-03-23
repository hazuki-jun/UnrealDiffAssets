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
	void RefreshRowNumberColumnWidth();
	
	TSharedRef<SWidget> BuildLayoutWidget(bool bIsLocal, TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> InListView);

	TSharedPtr<SHeaderRow> GenerateHeaderWidgets();

	float GetRowNumberColumnWidth() const;

	void OnRowNumberColumnResized(const float NewWidth);
	
	TSharedRef<ITableRow> MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable);
	
protected:
	/** Width of the row name column */
	float RowNameColumnWidth = 1.f;

	/** Width of the row number column */
	float RowNumberColumnWidth = 1.f;
	
	TSharedPtr<SWindow> ParentWindow;

	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;

	TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> CellsListView_Local;

	TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> CellsListView_Remote;

	/** Array of the rows that are available for editing */
	TArray<FDataTableEditorRowListViewDataPtr> AvailableRows;

	/** Array of the columns that are available for editing */
	TArray<FDataTableEditorColumnHeaderDataPtr> AvailableColumns;
	
	/** Array of the rows that match the active filter(s) */
	TArray<FUnrealDiffDataTableRowListViewDataPtr> VisibleRows_Local;

	TArray<FUnrealDiffDataTableRowListViewDataPtr> VisibleRows_Remote;
};
