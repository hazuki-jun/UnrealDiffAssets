// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "PropertyViewWidgets/SUnrealDiffDetailTableRowBase.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDetailExpanderArrow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailExpanderArrow::Construct(const FArguments& InArgs, TSharedPtr<class SUnrealDiffDetailTableRowBase> RowData)
{
	Row = RowData;
	OnExpanderClicked = InArgs._OnExpanderClicked;
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
			.OnClicked(this, &SUnrealDiffDetailExpanderArrow::HandleOnExpanderClicked)
			.ContentPadding(0)
			.IsFocusable(false)
			[
				SNew(SImage)
				.Image(this, &SUnrealDiffDetailExpanderArrow::GetExpanderImage)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				// SNew(STextBlock)
				// .Text(FText::FromString(TEXT("Arrow")))
			]
		]
	];

}

EVisibility SUnrealDiffDetailExpanderArrow::GetExpanderVisibility() const
{
	TSharedPtr<SUnrealDiffDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return EVisibility::Hidden;
	}
	
	return RowPtr->DoesItemHaveChildren() ? EVisibility::Visible : EVisibility::Hidden;
}

const FSlateBrush* SUnrealDiffDetailExpanderArrow::GetExpanderImage() const
{
	TSharedPtr<SUnrealDiffDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return FUnrealDiffWindowStyle::GetAppSlateBrush("NoBrush");
	}

	FName ResourceName;
	if (bIsExpanded)
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName ExpandedHoveredName = "TreeArrow_Expanded_Hovered";
			ResourceName = ExpandedHoveredName;
		}
		else
		{
			static const FName ExpandedName = "TreeArrow_Expanded";
			ResourceName = ExpandedName;
		}
	}
	else
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

FReply SUnrealDiffDetailExpanderArrow::HandleOnExpanderClicked()
{
	bIsExpanded = !bIsExpanded;
	
	TSharedPtr<SUnrealDiffDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return FReply::Unhandled();
	}
	
	const FModifierKeysState ModKeyState = FSlateApplication::Get().GetModifierKeys();
	if (!ModKeyState.IsShiftDown())
	{
		RowPtr->ToggleExpansion();
	}
	OnExpanderClicked.ExecuteIfBound(bIsExpanded);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
