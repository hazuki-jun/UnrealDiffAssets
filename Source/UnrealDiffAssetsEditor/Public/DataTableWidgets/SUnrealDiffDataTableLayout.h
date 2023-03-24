﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "SUnrealDiffDataTableListViewRow.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableLayout : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableLayout){}
		SLATE_ARGUMENT(FText, Title)
		SLATE_ARGUMENT(UObject*, AssetObject)
		SLATE_ARGUMENT(bool, IsLocal)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> BuildContent();
	
	TSharedPtr<SHeaderRow> GenerateHeaderWidgets();
	
	void SetupColumnWidth();
	
	float GetRowNameColumnWidth() const;
	void RefreshRowNameColumnWidth();

	float GetRowNumberColumnWidth() const;
	void RefreshRowNumberColumnWidth();

	float GetColumnWidth(const int32 ColumnIndex) const;

	void OnColumnResized(const float NewWidth, const int32 ColumnIndex);

	void OnRowNameColumnResized(const float NewWidth);

	void OnRowNumberColumnResized(const float NewWidth);
	
	TSharedRef<ITableRow> MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable);

	void OnRowSelectionChanged(FUnrealDiffDataTableRowListViewDataPtr InNewSelection, ESelectInfo::Type InSelectInfo);
	
protected:
	/** Struct holding information about the current column widths */
	struct FColumnWidth
	{
		FColumnWidth()
			: bIsAutoSized(true)
			, CurrentWidth(0.0f)
		{
		}

		/** True if this column is being auto-sized rather than sized by the user */
		bool bIsAutoSized;
		/** The width of the column, either sized by the user, or auto-sized */
		float CurrentWidth;
	};
	
	/** Width of the row name column */
	float RowNameColumnWidth = 1.f;

	/** Width of the row number column */
	float RowNumberColumnWidth = 1.f;

	/** Widths of data table cell columns */
	TArray<FColumnWidth> ColumnWidths;
	
	/** Array of the rows that match the active filter(s) */
	TArray<FUnrealDiffDataTableRowListViewDataPtr> VisibleRows;
	
	/** Array of the rows that are available for editing */
	TArray<FDataTableEditorRowListViewDataPtr> AvailableRows;

	/** Array of the columns that are available for editing */
	TArray<FDataTableEditorColumnHeaderDataPtr> AvailableColumns;
	
	UObject* AssetObject = nullptr;

	TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> ListView;

	bool bIsLocal;
};