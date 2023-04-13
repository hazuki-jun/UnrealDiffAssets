// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealDiffStringTableEntry.h"
#include "Widgets/Views/STableRow.h"

/**
 * 
 */
class SUnrealDiffStringTableEntryRow : public SMultiColumnTableRow<TSharedPtr<FCachedStringTableEntry>>
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffStringTableEntryRow) {}
		SLATE_ARGUMENT(bool, IsLocal)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<FCachedStringTableEntry> InCachedStringTableEntry, TSharedPtr<class SUnrealDiffStringTableListView> InStringTableListView);
	
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	TSharedRef<SWidget> MakeRowActionsMenu();

	void OnMenuActionCopyName();
	void OnMenuActionCopyValue();
	void OnMenuActionMerge();

protected:
	TSharedPtr<class SUnrealDiffStringTableListView> StringTableListView;
	TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry;
	bool bIsLocal = true;
};
