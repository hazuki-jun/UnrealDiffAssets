// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailCategoryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TWeakPtr<class FUnrealDiffDetailTreeNode> OwnerTreeNode_)
{
	OwnerTreeNode = OwnerTreeNode_;
	const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& ChildNodes = OwnerTreeNode.Pin()->GetChildNodes();
	bool bHasAnyDifference = false;
	bHasAnyDifference = OwnerTreeNode.Pin()->HasAnyDifferenceRecurse();
	// for (int32 i = 0; i < ChildNodes.Num(); ++i)
	// {
	// 	if (ChildNodes[i] && ChildNodes[i]->bHasAnyDifference)
	// 	{
	// 		OwnerTreeNode.Pin()->bHasAnyDifference = true;
	// 		bHasAnyDifference = true;
	// 		break;
	// 	}
	// }
	
	if (bHasAnyDifference)
	{
		DisplayNameColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else
	{
		DisplayNameColor = FLinearColor(1.f, 1.f, 1.f, 1.f);	
	}
	
	TSharedRef<SHorizontalBox> HeaderBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0, 0, 0, 0)
		.AutoWidth()
		[
			SNew(SUnrealDiffDetailExpanderArrow, SharedThis(this))
			.OnExpanderClicked(this, &SUnrealDiffDetailCategoryRow::OnExpanderClicked)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(4, 0, 0, 0)
		.FillWidth(1)
		[
			SNew(STextBlock)
			.Text(FText::FromName(InArgs._CategoryName))
			.TextStyle(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.CategoryTextStyle")
			.ColorAndOpacity(this, &SUnrealDiffDetailCategoryRow::GetDisplayNameColor)
		];
	
	this->ChildSlot
	[
		SNew(SBorder)
		.BorderImage(this, &SUnrealDiffDetailCategoryRow::GetBackgroundImage)
		.Padding(FMargin(0, 0, 0, 0))
		[
			SNew(SBox)
			.MinDesiredHeight(26.f)
			[
				HeaderBox
			]
		]
	];

	STableRow< TSharedPtr< FUnrealDiffDetailTreeNode > >::ConstructInternal(
	STableRow::FArguments()
		.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.TreeView.TableRow")
		.ShowSelection(false),
		InOwnerTableView);
}

void SUnrealDiffDetailCategoryRow::OnExpanderClicked(bool bIsExpanded)
{
	if (!OwnerTreeNode.IsValid())
	{
		return;
	}

	auto DetailView = OwnerTreeNode.Pin()->GetDetailsView();
	if (!DetailView)
	{
		return;
	}

	auto DataTableVisual = DetailView->GetDataTableVisualDiff();
	
	if (!DataTableVisual)
	{
		return;
	}
	
	DataTableVisual->SyncDetailViewAction_Expanded(DetailView->IsLocalAsset(), bIsExpanded, OwnerTreeNode.Pin()->GetNodeIndex());
}

void SUnrealDiffDetailCategoryRow::Refresh()
{
	bool bHasAnyDifference = false;
	bHasAnyDifference = OwnerTreeNode.Pin()->HasAnyDifferenceRecurse();
	// for (int32 i = 0; i < ChildNodes.Num(); ++i)
	// {
	// 	if (ChildNodes[i] && ChildNodes[i]->bHasAnyDifference)
	// 	{
	// 		OwnerTreeNode.Pin()->bHasAnyDifference = true;
	// 		bHasAnyDifference = true;
	// 		break;
	// 	}
	// }
	
	if (bHasAnyDifference)
	{
		DisplayNameColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else
	{
		DisplayNameColor = FLinearColor(1.f, 1.f, 1.f, 1.f);	
	}
}

const FSlateBrush* SUnrealDiffDetailCategoryRow::GetBackgroundImage() const
{
	if (IsHovered())
	{
		return IsItemExpanded() ? FEditorStyle::GetBrush("DetailsView.CategoryTop_Hovered") : FEditorStyle::GetBrush("DetailsView.CollapsedCategory_Hovered");
	}
	else
	{
		return IsItemExpanded() ? FEditorStyle::GetBrush("DetailsView.CategoryTop") : FEditorStyle::GetBrush("DetailsView.CollapsedCategory");
	}
	
	// return FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.CategoryTop");
	
	// if (bShowBorder)
	// {
	// 	if (bIsInnerCategory)
	// 	{
	// 		return FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle");
	// 	}
	//
	// 	// intentionally no hover on outer categories
	// 	return FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.CategoryTop");
	// }
	//
	// return nullptr;
}

FSlateColor SUnrealDiffDetailCategoryRow::GetInnerBackgroundColor() const
{
	return FSlateColor(FLinearColor::White);
}

FSlateColor SUnrealDiffDetailCategoryRow::GetOuterBackgroundColor() const
{
	if (IsHovered())
	{
		return FUnrealDiffWindowStyle::GetAppSlateColor("Colors.Header");
	}

	return FUnrealDiffWindowStyle::GetAppSlateColor("Colors.Panel");
}

FSlateColor SUnrealDiffDetailCategoryRow::GetDisplayNameColor() const
{
	return DisplayNameColor;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
