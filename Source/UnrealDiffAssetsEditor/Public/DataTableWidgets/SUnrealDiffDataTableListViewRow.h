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

	bool bIsRemoved = false;

	bool bIsAdded = false;

	bool bHasAnyDifference = false;
};

typedef TSharedPtr<FUnrealDiffDataTableRowListViewData> FUnrealDiffDataTableRowListViewDataPtr;

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableListViewRow : public SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableListViewRow){}
		SLATE_ARGUMENT(bool, IsLocal)
		SLATE_ARGUMENT(TSharedPtr<class SDataTableVisualDiff>, DataTableVisual)
		SLATE_ARGUMENT(TSharedPtr<class SUnrealDiffDataTableLayout>, DataTableLayout)
		SLATE_ARGUMENT(FUnrealDiffDataTableRowListViewDataPtr, InRowDataPtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	//~ Begin SMultiColumnTableRow Interface
	virtual const FSlateBrush* GetBorder() const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnId) override;
	//~ End SMultiColumnTableRow Interface

	TSharedRef<SWidget> MakeCellWidget(const FName& InColumnId);

	TSharedRef<SWidget> MakeRowActionsMenu();

	void OnMenuActionCopyName();
	void OnMenuActionCopyValue();
	void OnMenuActionShowDifference();
	
	bool bIsLocal = true;
	FUnrealDiffDataTableRowListViewDataPtr RowDataPtr;
	TSharedPtr<class SDataTableVisualDiff> DataTableVisual = nullptr;
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayout;
};


