// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyNameWidget.h"

#include "SlateOptMacros.h"
#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffPropertyNameWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyNameWidget::Construct(const FArguments& InArgs, TWeakPtr<class FUnrealDiffDetailTreeNode> OwnerNode)
{
	FText DisplayNameText;
	if (OwnerNode.Pin()->bIsInContainer)
	{
		DisplayNameText = FText::FromString( FString::Format(TEXT("[{0}]"), { OwnerNode.Pin()->PropertyIndex }) );
	}
	else
	{
		DisplayNameText = OwnerNode.Pin()->Property.Get()->GetDisplayNameText(); 
	}

	FSlateColor SlateColor;
	if (OwnerNode.Pin()->bHasAnyDifference)
	{
		SlateColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else
	{
		SlateColor = FLinearColor(1.f, 1.f, 1.f, 1.f);	
	}
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(DisplayNameText)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.ColorAndOpacity(SlateColor)
		]
	];
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
