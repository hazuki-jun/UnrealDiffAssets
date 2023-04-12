// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualDiffWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SStringTableVisualDiff : public SVisualDiffWidget
{
public:
	SLATE_BEGIN_ARGS(SStringTableVisualDiff){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//~ Begin SVisualDiffWidget Toolbar Action Interface
	// Next
	virtual void ToolbarAction_HighlightNextDifference() override;
	// Prev
	virtual void ToolbarAction_HighlightPrevDifference() override;
	// Diff
	virtual void ToolbarAction_Diff() override;
	virtual bool ToolbarAction_CanDiff() override;
	// Merge
	virtual void ToolbarAction_Merge() override;
	//~ End SVisualDiffWidget Toolbar Action Interface

	// Begin SVisualDiffWidget ViewOption Interface
	virtual void RefreshLayout() override;
	virtual void ModifyConfig() override;
	// End SVisualDiffWidget ViewOption Interface
	
protected:
	TSharedPtr<SWindow> ParentWindow;
	
	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;
};
