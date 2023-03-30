// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailExpanderArrow::Construct(const FArguments& InArgs, TSharedPtr<class SUnrealDiffDetailTableRowBase> RowData)
{
	ChildSlot
	[
	SNew(SBox)
		.Visibility(this, &SUnrealDiffDetailExpanderArrow::GetExpanderVisibility)
		[
			SAssignNew(ExpanderArrow, SButton)
			.ButtonStyle(FCoreStyle::Get(), "NoBorder")
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.ClickMethod(EButtonClickMethod::MouseDown)
			.OnClicked(this, &SUnrealDiffDetailExpanderArrow::OnExpanderClicked)
			.ContentPadding(0)
			.IsFocusable(false)
			[
				SNew(SImage)
				.Image(this, &SUnrealDiffDetailExpanderArrow::GetExpanderImage)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			]
		]
	];

}

EVisibility SUnrealDiffDetailExpanderArrow::GetExpanderVisibility() const
{
	TSharedPtr<SUnrealDiffDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return EVisibility::Collapsed;
	}

	// return RowPtr->DoesItemHaveChildren() ? EVisibility::Visible : EVisibility::Hidden;
	return EVisibility::Visible;
}

const FSlateBrush* SUnrealDiffDetailExpanderArrow::GetExpanderImage() const
{
	TSharedPtr<SUnrealDiffDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return FUnrealDiffWindowStyle::GetAppSlateBrush("NoBrush");
	}

	FName ResourceName;
	// const bool bIsItemExpanded = RowPtr->IsItemExpanded();
	// if (bIsItemExpanded)
	// {
	// 	if (ExpanderArrow->IsHovered())
	// 	{
	// 		static const FName ExpandedHoveredName = "TreeArrow_Expanded_Hovered";
	// 		ResourceName = ExpandedHoveredName;
	// 	}
	// 	else
	// 	{
	// 		static const FName ExpandedName = "TreeArrow_Expanded";
	// 		ResourceName = ExpandedName;
	// 	}
	// }
	// else
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName CollapsedHoveredName = "TreeArrow_Collapsed_Hovered";
			ResourceName = CollapsedHoveredName;
		}
		else
		{
			static const FName CollapsedName = "TreeArrow_Collapsed";
			ResourceName = CollapsedName;
		}
	}

	return FUnrealDiffWindowStyle::GetAppSlateBrush(ResourceName);
}

FReply SUnrealDiffDetailExpanderArrow::OnExpanderClicked()
{
	return FReply::Unhandled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
