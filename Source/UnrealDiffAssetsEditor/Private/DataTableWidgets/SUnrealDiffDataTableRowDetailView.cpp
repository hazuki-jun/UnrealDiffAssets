// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"

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

	SAssignNew(DetailTree, SUnrealDiffDataTableDetailTree);
	
	this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			BuildRowTitle()
		]

		+ SVerticalBox::Slot()
		[
			DetailTree.ToSharedRef()
		]
	];
}

TSharedRef<SWidget> SUnrealDiffDataTableRowDetailView::BuildRowTitle()
{
	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
			.BorderImage(FUnrealDiffWindowStyle::GetAppSlateBrush("Brushes.Title"))
		]

		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FUnrealDiffWindowStyle::GetAppSlateBrush("Brushes.Background"))
				.VAlign(VAlign_Bottom)
				.Padding(0.0f)
				[
					SNew(STextBlock)
					.Text(this, &SUnrealDiffDataTableRowDetailView::RowTitle)
				]
			]
		];
}

void SUnrealDiffDataTableRowDetailView::Refresh(const FName& InRowName)
{
	if (!DetailTree.IsValid())
	{
		return;
	}

	if (DataTableVisualDiff)
	{
		RowName = InRowName;
		DetailTree->SetStructure(DataTableVisualDiff->GetStructure());
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
}

FText SUnrealDiffDataTableRowDetailView::RowTitle() const
{
	return FText::FromName(RowName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
