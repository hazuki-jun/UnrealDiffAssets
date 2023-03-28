// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableRowDetailView::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisualDiff = InArgs._DataTableVisualDiff;
	check(DataTableVisualDiff)
	
	if (bIsLocal)
	{
		DataTableVisualDiff->RowDetailViewLocal = SharedThis(this);
	}
	else
	{
		DataTableVisualDiff->RowDetailViewRemote = SharedThis(this);
	}
	
	// static const FSlateBrush* MajorTabBackgroundBrush = FAppStyle::Get().GetBrush("Brushes.Title");
	// static const FSlateBrush* MinorTabBackgroundBrush = FAppStyle::Get().GetBrush("Brushes.Background");
	// return bShowingTitleBarArea ? MajorTabBackgroundBrush : MinorTabBackgroundBrush;
	
	this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			// tab well area
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Title"))
			.VAlign(VAlign_Bottom)
			.Padding(0.0f)
		]
	];
}

void SUnrealDiffDataTableRowDetailView::Refresh(const FName& RowName)
{
	SetVisibility(EVisibility::SelfHitTestInvisible);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
