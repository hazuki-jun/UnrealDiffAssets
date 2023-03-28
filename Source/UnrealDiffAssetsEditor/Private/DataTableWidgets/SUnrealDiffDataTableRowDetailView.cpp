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

	// UDataTable* DataTable = Cast<UDataTable>(DataTableVisualDiff->GetAssetObject(bIsLocal)); 
	
	// static const FSlateBrush* MajorTabBackgroundBrush = FAppStyle::Get().GetBrush("Brushes.Title");
	// static const FSlateBrush* MinorTabBackgroundBrush = FAppStyle::Get().GetBrush("Brushes.Background");
	// return bShowingTitleBarArea ? MajorTabBackgroundBrush : MinorTabBackgroundBrush;
	
	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("Brushes.Title"))
		]

		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Background"))
				.VAlign(VAlign_Bottom)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(this, &SUnrealDiffDataTableRowDetailView::RowTitle)
				]
			]
		]
	];
}

void SUnrealDiffDataTableRowDetailView::Refresh(const FName& InRowName)
{
	RowName = InRowName;
	SetVisibility(EVisibility::SelfHitTestInvisible);
}

FText SUnrealDiffDataTableRowDetailView::RowTitle() const
{
	return FText::FromName(RowName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
