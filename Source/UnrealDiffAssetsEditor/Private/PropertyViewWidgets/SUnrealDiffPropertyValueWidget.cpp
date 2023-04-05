// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"

#include "SlateOptMacros.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffPropertyValueWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyValueWidget::Construct(const FArguments& InArgs, TWeakPtr<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	auto Font = FCoreStyle::GetDefaultFontStyle("Regular", 8);
	
	OwnerTreeNode = InOwnerTreeNode;
	const FProperty* Property = OwnerTreeNode.Pin().Get()->Property.Get();
	ValueText = GetValueText(Property);
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
		ValueText = GetValueText(OwnerTreeNode.Pin().Get()->Property.Get());
	}
}

FText SUnrealDiffPropertyValueWidget::GetText() const
{
	return ValueText;
}

FText SUnrealDiffPropertyValueWidget::GetValueText(const FProperty* InProperty)
{
	FText OutText;

	if (!OwnerTreeNode.IsValid())
	{
		return OutText;
	}

	if (OwnerTreeNode.Pin()->bIsInContainer)
	{
		return GetValueTextInContainer(InProperty);
	}
	
	const void* StructData = OwnerTreeNode.Pin()->GetStructData(0);
	OutText = GetValueTextFromStructData(StructData, InProperty);
	return OutText;
}

FText SUnrealDiffPropertyValueWidget::GetValueTextInContainer(const FProperty* InProperty)
{
	FText OutText;

	if (!OwnerTreeNode.IsValid() || !InProperty)
	{
		return OutText;
	}

	if (CastField<const FArrayProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		return OutText;
	}
	else if (CastField<const FSetProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		return OutText;
	}
	else if (CastField<const FMapProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		return OutText;
	}
	else if (CastField<const FStructProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		return OutText;
	}
	
	auto StructData = OwnerTreeNode.Pin()->GetStructData();
	
	const FProperty* ContainerProperty = OwnerTreeNode.Pin()->ContainerProperty.Get();
	if (const FArrayProperty* ArrayProp = CastField<const FArrayProperty>(ContainerProperty))
	{
		return GetValueTextFromStructData(OwnerTreeNode.Pin()->RowDataInContainer, InProperty);
		// FScriptArrayHelper ArrayHelper(ArrayProp, StructData);
		// if (ArrayHelper.IsValidIndex(OwnerTreeNode.Pin()->PropertyIndex))
		// {
		// 	const uint8* ArrayEntryData = ArrayHelper.GetRawPtr(OwnerTreeNode.Pin()->PropertyIndex);
		// 	return GetValueTextFromStructData(ArrayEntryData, InProperty);
		// }	
	}
	else if (const FSetProperty* SetProp = CastField<const FSetProperty>(ContainerProperty))
	{
		return GetValueTextFromStructData(OwnerTreeNode.Pin()->RowDataInContainer, InProperty);
		// FScriptSetHelper SetHelper(SetProp, StructData);
		// if (SetHelper.IsValidIndex(OwnerTreeNode.Pin()->PropertyIndex))
		// {
		// 	const uint8* SetEntryData = SetHelper.GetElementPtr(OwnerTreeNode.Pin()->PropertyIndex);
		// 	return GetValueTextFromStructData(SetEntryData, InProperty);
		// }	
	}
	else if (const FMapProperty* MapProp = CastField<const FMapProperty>(ContainerProperty))
	{

	}
	
	return OutText;
}

FText SUnrealDiffPropertyValueWidget::GetValueTextFromStructData(const void* InStructData, const FProperty* InProperty)
{
	FText OutText;

	if (!InStructData || !InProperty)
	{
		return OutText;
	}
	
	if (const FEnumProperty* EnumProp = CastField<const FEnumProperty>(InProperty))
	{
		OutText = DataTableUtils::GetPropertyValueAsText(EnumProp, (uint8*)InStructData);
	}
	else if (const FNumericProperty *NumProp = CastField<const FNumericProperty>(InProperty))
	{
		if (NumProp->IsEnum())
		{
			OutText = DataTableUtils::GetPropertyValueAsText(EnumProp, (uint8*)InStructData);
		}
		else if (NumProp->IsInteger())
		{
			if (InStructData)
			{
				auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(InStructData, 0);
				const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
				OutText = FText::FromString(FString::FromInt(PropertyValue));
			}
		}
		else if (NumProp->IsA(FFloatProperty::StaticClass()))
		{
			OutText = DataTableUtils::GetPropertyValueAsText(NumProp, (uint8*)InStructData);
			if (OutText.IsEmpty())
			{
				OutText = FText::FromString(TEXT("0.0"));
			}
			// const float PropertyValue = (float)NumProp->GetFloatingPointPropertyValue(InStructData);
			// OutText = FText::FromString(FString::Format(TEXT("{0}"), { PropertyValue }));
		}
		else
		{
			OutText = DataTableUtils::GetPropertyValueAsText(NumProp, (uint8*)InStructData);
			if (OutText.IsEmpty())
			{
				OutText = FText::FromString(TEXT("0.0"));
			}
			
			// const double PropertyValue = NumProp->GetFloatingPointPropertyValue(InStructData);
			// OutText = FText::FromString(FString::Format(TEXT("{0}"), { PropertyValue }));
		}
	}
	else if (const FBoolProperty* BoolProp = CastField<const FBoolProperty>(InProperty))
	{
		const bool PropertyValue = BoolProp->GetPropertyValue(InStructData);
		OutText = FText::FromString(PropertyValue ? TEXT("true") : TEXT("false"));
	}
	else if (const FArrayProperty* ArrayProp = CastField<const FArrayProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		// JsonWriter->WriteArrayStart(Identifier);
		//
		// FScriptArrayHelper ArrayHelper(ArrayProp, InStructData);
		// for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < ArrayHelper.Num(); ++ArrayEntryIndex)
		// {
		// 	const uint8* ArrayEntryData = ArrayHelper.GetRawPtr(ArrayEntryIndex);
		// 	WriteContainerEntry(ArrayProp->Inner, ArrayEntryData);
		// }
		//
		// JsonWriter->WriteArrayEnd();
	}
	else if (const FSetProperty* SetProp = CastField<const FSetProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		// JsonWriter->WriteArrayStart(Identifier);
		//
		// FScriptSetHelper SetHelper(SetProp, InStructData);
		// for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
		// {
		// 	if (SetHelper.IsValidIndex(SetSparseIndex))
		// 	{
		// 		const uint8* SetEntryData = SetHelper.GetElementPtr(SetSparseIndex);
		// 		WriteContainerEntry(SetHelper.GetElementProperty(), SetEntryData);
		// 	}
		// }
		//
		// JsonWriter->WriteArrayEnd();
	}
	else if (const FMapProperty* MapProp = CastField<const FMapProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
		// JsonWriter->WriteObjectStart(Identifier);
		//
		// FScriptMapHelper MapHelper(MapProp, StructData);
		// for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
		// {
		// 	if (MapHelper.IsValidIndex(MapSparseIndex) && OwnerTreeNode.Pin()->PropertyIndex == MapSparseIndex)
		// 	{
		// 		const uint8* MapKeyData = MapHelper.GetKeyPtr(MapSparseIndex);
		// 		const uint8* MapValueData = MapHelper.GetValuePtr(MapSparseIndex);
		// 		OutText = DataTableUtils::GetPropertyValueAsText(MapHelper.GetValueProperty(), MapValueData);
		// 		// OutText = DataTableUtils::GetPropertyValueAsText(MapHelper.GetKeyProperty(), MapKeyData);
		// 		break;
		// 	}
		// }
	}
	else if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
	{
		SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		OutText = DataTableUtils::GetPropertyValueAsText(InProperty, (uint8*)InStructData);
		if (OutText.IsEmpty())
		{
			OutText = FText::FromString(TEXT("None"));
		}
	}
	
	return OutText;	
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
