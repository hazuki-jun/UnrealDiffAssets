// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"

#include "SlateOptMacros.h"
#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffPropertyValueWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyValueWidget::Construct(const FArguments& InArgs, TWeakPtr<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	if (!OwnerTreeNode.IsValid())
	{
		return;
	}
	auto Font = FCoreStyle::GetDefaultFontStyle("Regular", 8);
	ValueText = OwnerTreeNode.Pin()->GetValueText();
	if (ValueText.IsEmpty() || OwnerTreeNode.Pin()->IsContainerNode())
	{
		return;
	}
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.Padding(FMargin(10.f, 0.f, 10.f, 0.f))
			[
				SNew(SEditableTextBox)
				.IsReadOnly(true)
				.BackgroundColor(FSlateColor(FLinearColor(0.f, 0.f, 0.f)))
				.Text(this, &SUnrealDiffPropertyValueWidget::GetText)
			]
		]
	];
}

void SUnrealDiffPropertyValueWidget::Refresh()
{
	if (OwnerTreeNode.Pin().Get())
	{
		ValueText = OwnerTreeNode.Pin()->GetValueText();
	}
}

FText SUnrealDiffPropertyValueWidget::GetText() const
{
	return ValueText;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
