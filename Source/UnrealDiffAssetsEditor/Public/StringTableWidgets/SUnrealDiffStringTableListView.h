// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"



/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffStringTableListView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffStringTableListView) {}
		SLATE_ARGUMENT(bool, IsLocal)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedPtr<class SStringTableVisualDiff> InStringTableVisualDiff);

	void Refresh();
	
	void SetupRowsListSources();
	
public:
	/** Cached string table entry */
	struct FCachedStringTableEntry
	{
		FCachedStringTableEntry(FString InKey, FString InSourceString)
			: Key(MoveTemp(InKey))
			, SourceString(MoveTemp(InSourceString))
			, RowState(0)
		{
		}

		FString Key;
		FString SourceString;
		int32 RowState;
	};
	TArray<TSharedPtr<FCachedStringTableEntry>> CachedStringTableEntries;

protected:
	/** Create the row for the given cached string table entry */
	TSharedRef<ITableRow> OnGenerateStringTableEntryRow(TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry, const TSharedRef<STableViewBase>& Table);
	
protected:
	bool bIsLocal = true;
	
	TSharedPtr<class SStringTableVisualDiff> StringTableVisualDiff; 
	
	/** List view showing the cached string table entries */
	TSharedPtr<SListView<TSharedPtr<FCachedStringTableEntry>>> MyListView;
};

class SUnrealDiffStringTableEntryRow : public SMultiColumnTableRow<TSharedPtr<SUnrealDiffStringTableListView::FCachedStringTableEntry>>
{
public:
	void Construct(const FTableRowArgs& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<SUnrealDiffStringTableListView::FCachedStringTableEntry> InCachedStringTableEntry);
	
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	TSharedRef<SWidget> MakeRowActionsMenu();

	void OnMenuActionCopyName();
	void OnMenuActionCopyValue();
	
	TSharedPtr<SUnrealDiffStringTableListView::FCachedStringTableEntry> CachedStringTableEntry;
};
