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
	
	TSharedRef<SWidget> BuildLayoutWidget(bool bIsLocal);

	TSharedRef<SWidget> GenerateHeaderWidgets();

	TSharedRef<ITableRow> MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable);
	
protected:
	TSharedPtr<SWindow> ParentWindow;

	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;

	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow;
	
	TSharedPtr<SSearchBox> SearchBoxWidget;

	TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> CellsListView;

	/** Array of the rows that are available for editing */
	TArray<FDataTableEditorRowListViewDataPtr> AvailableRows;

	/** Array of the columns that are available for editing */
	TArray<FDataTableEditorColumnHeaderDataPtr> AvailableColumns;
	
	/** Array of the rows that match the active filter(s) */
	TArray<FUnrealDiffDataTableRowListViewDataPtr> VisibleRows_Local;

	TArray<FUnrealDiffDataTableRowListViewDataPtr> VisibleRows_Remote;
};
