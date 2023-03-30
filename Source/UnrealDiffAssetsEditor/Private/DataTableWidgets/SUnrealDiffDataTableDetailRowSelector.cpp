// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableDetailRowSelector.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableDetailRowSelector"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableDetailRowSelector::Construct(const FArguments& InArgs)
{
	for (const auto& Row : InArgs._Rows)
	{
		Rows.Add(MakeShared<FName>(Row));
	}
	
	SelectedRowText = FText::FromName(InArgs._SelectedRowName);
	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(170)
		.ToolTipText(LOCTEXT("SelectedRowTooltip", "Select a row to diff"))
		[
			SAssignNew(MyComboBox, SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&Rows)
			.OnSelectionChanged(this, &SUnrealDiffDataTableDetailRowSelector::OnSelectionChanged)
			.OnGenerateWidget(this, &SUnrealDiffDataTableDetailRowSelector::OnGenerateComboWidget)
			.Content()
			[
				SNew(STextBlock).Text(this, &SUnrealDiffDataTableDetailRowSelector::GetCurrentName)
			]
		]
	];
}

void SUnrealDiffDataTableDetailRowSelector::OnSelectionChanged(TSharedPtr<FName> InItem, ESelectInfo::Type InSelectionInfo)
{
	// SelectedRowText = FText::FromName(*InItem.Get());
}

TSharedRef<SWidget> SUnrealDiffDataTableDetailRowSelector::OnGenerateComboWidget(TSharedPtr<FName> InItem)
{
	return SNew(STextBlock).Text(FText::FromName(InItem.IsValid() ? *InItem : NAME_None));
}

FText SUnrealDiffDataTableDetailRowSelector::GetCurrentName() const
{
	return SelectedRowText;
}

void SUnrealDiffDataTableDetailRowSelector::OnRowSelected(FName RowName)
{
	SelectedRowText = FText::FromName(RowName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
