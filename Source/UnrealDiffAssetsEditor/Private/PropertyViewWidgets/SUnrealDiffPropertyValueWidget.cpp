// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"

#include "SlateOptMacros.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffPropertyValueWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyValueWidget::Construct(const FArguments& InArgs, TWeakPtr<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;

	FText Value;
	
	const FProperty* Property = OwnerTreeNode.Pin().Get()->PropertyData->Property.Get();
	if (Property)
	{
		if (auto StructProperty = CastField<FStructProperty>(Property))
		{
			auto CppType = StructProperty->GetCPPType(nullptr, 0);
			if (CppType.Equals(TEXT("FVector"), ESearchCase::IgnoreCase))
			{
				
			}
			else if (CppType.Equals(TEXT("FVector2D"), ESearchCase::IgnoreCase))
			{
				
			}
			else
			{
				
			}
		}
		else if (CastField<FArrayProperty>(Property))
		{
			
		}
		else if (CastField<FSetProperty>(Property))
		{
			
		}
		else if (CastField<FMapProperty>(Property))
		{
			
		}
		else
		{
			const auto RowData = OwnerTreeNode.Pin().Get()->PropertyData->RowData;
			Value = DataTableUtils::GetPropertyValueAsText(Property, RowData);	
		}
	}
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			[
				SNew(STextBlock)
				.Text(Value)
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
