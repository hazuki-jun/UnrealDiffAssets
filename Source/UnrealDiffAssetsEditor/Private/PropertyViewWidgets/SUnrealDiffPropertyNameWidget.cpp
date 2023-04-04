// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyNameWidget.h"

#include "SlateOptMacros.h"
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
		DisplayNameText = OwnerNode.Pin()->Property->GetDisplayNameText(); 
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
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
