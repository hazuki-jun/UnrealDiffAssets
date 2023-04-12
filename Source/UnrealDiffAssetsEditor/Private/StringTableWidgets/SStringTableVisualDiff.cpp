// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SStringTableVisualDiff.h"

#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SStringTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference StringTable")));
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}

	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				MakeToolbar()
			]
		]
	];
}

void SStringTableVisualDiff::ToolbarAction_HighlightNextDifference()
{
	
}

void SStringTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	
}

void SStringTableVisualDiff::ToolbarAction_Diff()
{
	
}

bool SStringTableVisualDiff::ToolbarAction_CanDiff()
{
	return false;
}

void SStringTableVisualDiff::ToolbarAction_Merge()
{
	
}

void SStringTableVisualDiff::RefreshLayout()
{
}

void SStringTableVisualDiff::ModifyConfig()
{
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

