// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailCategoryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	TSharedRef<SHorizontalBox> HeaderBox = SNew(SHorizontalBox)
		// + SHorizontalBox::Slot()
		// .HAlign(HAlign_Left)
		// .VAlign(VAlign_Fill)
		// .AutoWidth()
		// [
		// 	SNew(SDetailRowIndent, SharedThis(this))
		// ]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(2, 0, 0, 0)
		.AutoWidth()
		[
			SNew(SUnrealDiffDetailExpanderArrow, SharedThis(this))
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(4, 0, 0, 0)
		.FillWidth(1)
		[
			SNew(STextBlock)
			.Text(FText::FromName(InArgs._CategoryName))
			// .Text(InArgs._DisplayName)
			// .Font(FAppStyle::Get().GetFontStyle(bIsInnerCategory ? PropertyEditorConstants::PropertyFontStyle : PropertyEditorConstants::CategoryFontStyle))
			.TextStyle(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.CategoryTextStyle")
		];
	
	
	this->ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.GridLine"))
		.Padding(FMargin(0, 0, 0, 1))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SBorder)
				.BorderImage(this, &SUnrealDiffDetailCategoryRow::GetBackgroundImage)
				.BorderBackgroundColor(this, &SUnrealDiffDetailCategoryRow::GetInnerBackgroundColor)
				.Padding(0)
				[
					SNew(SBox)
					.MinDesiredHeight(26.f)
					[
						HeaderBox
					]
				]
			]
			// + SHorizontalBox::Slot()
			// .HAlign(HAlign_Right)
			// .AutoWidth()
			// [
			// 	SNew(SBorder)
			// 	.BorderImage_Lambda(GetScrollbarWellBrush)
			// 	.BorderBackgroundColor_Lambda(GetScrollbarWellTint)
			// 	.Padding(FMargin(0, 0, SDetailTableRowBase::ScrollBarPadding, 0))
			// ]
		]
	];

	STableRow< TSharedPtr< FUnrealDiffDetailTreeNode > >::ConstructInternal(
	STableRow::FArguments()
		.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.TreeView.TableRow")
		.ShowSelection(false),
		InOwnerTableView);
}

bool SUnrealDiffDetailCategoryRow::HasChildren() const
{
	return true;
}

const FSlateBrush* SUnrealDiffDetailCategoryRow::GetBackgroundImage() const
{
	return FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.CategoryTop");
	
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

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
