// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


namespace EUnrealVisualDiff
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

ENUM_CLASS_FLAGS(EUnrealVisualDiff::RowViewOption);

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SVisualDiffWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVisualDiffWidget) {}

	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	virtual TSharedRef<SWidget> MakeToolbar();
	
	virtual FText GetFilterText() const;
	virtual void OnFilterTextChanged(const FText& InFilterText);
	virtual void OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);
	virtual void UpdateVisibleRows();
	
	//~ Begin Toolbar Action Interface
	// Next
	virtual void ToolbarAction_HighlightNextDifference();
	// Prev
	virtual void ToolbarAction_HighlightPrevDifference();
	// Diff
	virtual bool IsDiffable();
	virtual void ToolbarAction_Diff();
	virtual bool ToolbarAction_CanDiff();
	// Merge
	virtual void ToolbarAction_Merge();
	virtual bool IsSearchable();
	//~ End Toolbar Action Interface
	
	// ViewOption Content
	virtual TSharedRef<SWidget> GetShowViewOptionContent();

	/** Called when show only view option is clicked */
	virtual void OnShowOnlyViewOptionClicked(EUnrealVisualDiff::RowViewOption InViewOption);
	virtual void RefreshLayout();
	virtual bool HasRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption) const;
	virtual void ClearRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption);
	virtual void SetRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption);
	virtual void SetRowViewOptionTo(EUnrealVisualDiff::RowViewOption InViewOption);
	virtual void ReverseRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption);
	virtual void ModifyConfig();
	
	// Modified Check State
	virtual bool IsShowOnlyRowViewOptionChecked(EUnrealVisualDiff::RowViewOption InViewOption) const;

protected:
	EUnrealVisualDiff::RowViewOption RowViewOption = EUnrealVisualDiff::Max;
	FText ActiveFilterText;
	TSharedPtr<SSearchBox> SearchBoxWidget;
};
