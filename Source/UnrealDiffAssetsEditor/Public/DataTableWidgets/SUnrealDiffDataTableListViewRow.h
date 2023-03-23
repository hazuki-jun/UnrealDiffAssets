// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "Widgets/SCompoundWidget.h"

struct FUnrealDiffDataTableRowListViewData
{
	/** Unique ID used to identify this row */
	FName RowId;

	/** Display name of this row */
	FText DisplayName;

	/** The calculated height of this row taking into account the cell data for each column */
	float DesiredRowHeight;

	/** Insertion number of the row */
	uint32 RowNum;

	/** Array corresponding to each cell in this row */
	TArray<FText> CellData;
};

typedef TSharedPtr<FUnrealDiffDataTableRowListViewData> FUnrealDiffDataTableRowListViewDataPtr;

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableListViewRow : public SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableListViewRow){}
		SLATE_ARGUMENT(FUnrealDiffDataTableRowListViewDataPtr, RowDataPtr)
		SLATE_ARGUMENT(bool, IsEditable)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	//~ Begin SMultiColumnTableRow Interface
	virtual const FSlateBrush* GetBorder() const override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	//~ End SMultiColumnTableRow Interface
	
	TSharedRef<SWidget> MakeCellWidget(const int32 InRowIndex, const FName& InColumnId);
	
	TSharedPtr<SInlineEditableTextBlock> InlineEditableText;

	TSharedPtr<FName> CurrentName;

	FUnrealDiffDataTableRowListViewDataPtr RowDataPtr;

	bool IsEditable;
	bool bIsDragDropObject;
	bool bIsHoveredDragTarget;
};
