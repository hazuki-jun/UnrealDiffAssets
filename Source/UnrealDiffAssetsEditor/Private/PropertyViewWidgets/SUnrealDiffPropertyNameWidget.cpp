// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyNameWidget.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyNameWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(STextBlock)
		.Text(InArgs._DisplayNameText)
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
