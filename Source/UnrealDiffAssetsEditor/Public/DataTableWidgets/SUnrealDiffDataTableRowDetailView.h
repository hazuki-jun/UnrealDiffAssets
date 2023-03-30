// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableRowDetailView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableRowDetailView) {}
		SLATE_ARGUMENT(bool, IsLocal)
		SLATE_ARGUMENT(TSharedPtr<class SDataTableVisualDiff>, DataTableVisualDiff)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> BuildRowTitle();
	
	void Refresh(const FName& InRowName);
	
protected:
	bool bIsLocal = true;

	TSharedPtr<class SUnrealDiffDataTableDetailRowSelector> RowSelector;
	
	TSharedPtr<class SDataTableVisualDiff> DataTableVisualDiff;

	TSharedPtr<class SUnrealDiffDataTableDetailTree> DetailTree;
	
	FName RowName;
};
