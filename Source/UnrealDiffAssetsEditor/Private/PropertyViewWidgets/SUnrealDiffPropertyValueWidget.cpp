// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"

#include "SlateOptMacros.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffPropertyValueWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffPropertyValueWidget::Construct(const FArguments& InArgs, TWeakPtr<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	const FProperty* Property = OwnerTreeNode.Pin().Get()->Property.Get();
	FText Value = GetValueText(Property);
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

FText SUnrealDiffPropertyValueWidget::GetValueText(const FProperty* InProperty)
{
	FText OutText;
	const void* StructData = OwnerTreeNode.Pin()->GetStructData(0);
	
	if (const FEnumProperty* EnumProp = CastField<const FEnumProperty>(InProperty))
	{
		// const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(EnumProp, (uint8*)InRowData, DTExportFlags);
		// JsonWriter->WriteValue(Identifier, PropertyValue);
	}
	else if (const FNumericProperty *NumProp = CastField<const FNumericProperty>(InProperty))
	{
		if (NumProp->IsEnum())
		{
			// const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, DTExportFlags);
			// JsonWriter->WriteValue(Identifier, PropertyValue);
		}
		else if (NumProp->IsInteger())
		{
			if (StructData)
			{
				auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(StructData, 0);
				const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
				OutText = FText::FromString(FString::FromInt(PropertyValue));
			}
			
			// if (OwnerTreeNode.Pin()->PropertyData->ParentStructProperty.Get())
			// {
			// 	auto ParentStructData = OwnerTreeNode.Pin()->PropertyData->ParentStructProperty->ContainerPtrToValuePtr<void>(StructData);
			// 	auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(ParentStructData, 0);
			// 	const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
			// 	OutText = FText::FromString(FString::FromInt(PropertyValue));
			// }
			// else
			// {
			// 	auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(StructData, 0);
			// 	const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
			// 	OutText = FText::FromString(FString::FromInt(PropertyValue));
			// }
			// JsonWriter->WriteValue(Identifier, PropertyValue);
		}
		else if (NumProp->IsA(FFloatProperty::StaticClass()))
		{
			// const float PropertyValue = (float)NumProp->GetFloatingPointPropertyValue(InPropertyData);
			// JsonWriter->WriteValue(Identifier, PropertyValue);
		}
		else
		{
			// const double PropertyValue = NumProp->GetFloatingPointPropertyValue(InPropertyData);
			// JsonWriter->WriteValue(Identifier, PropertyValue);
		}
	}
	else if (const FBoolProperty* BoolProp = CastField<const FBoolProperty>(InProperty))
	{
		// const bool PropertyValue = BoolProp->GetPropertyValue(InPropertyData);
		// JsonWriter->WriteValue(Identifier, PropertyValue);
	}
	else if (const FArrayProperty* ArrayProp = CastField<const FArrayProperty>(InProperty))
	{
		// JsonWriter->WriteArrayStart(Identifier);
		//
		// FScriptArrayHelper ArrayHelper(ArrayProp, InPropertyData);
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
		// JsonWriter->WriteArrayStart(Identifier);
		//
		// FScriptSetHelper SetHelper(SetProp, InPropertyData);
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
		// JsonWriter->WriteObjectStart(Identifier);
		//
		// FScriptMapHelper MapHelper(MapProp, InPropertyData);
		// for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
		// {
		// 	if (MapHelper.IsValidIndex(MapSparseIndex))
		// 	{
		// 		const uint8* MapKeyData = MapHelper.GetKeyPtr(MapSparseIndex);
		// 		const uint8* MapValueData = MapHelper.GetValuePtr(MapSparseIndex);
		//
		// 		// JSON object keys must always be strings
		// 		const FString KeyValue = DataTableUtils::GetPropertyValueAsStringDirect(MapHelper.GetKeyProperty(), (uint8*)MapKeyData, DTExportFlags);
		// 		WriteContainerEntry(MapHelper.GetValueProperty(), MapValueData, &KeyValue);
		// 	}
		// }
		//
		// JsonWriter->WriteObjectEnd();
	}
	else if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
	{
		// if (!!(DTExportFlags & EDataTableExportFlags::UseJsonObjectsForStructs))
		// {
		// 	JsonWriter->WriteObjectStart(Identifier);
		// 	WriteStruct(StructProp->Struct, InPropertyData);
		// 	JsonWriter->WriteObjectEnd();
		// }
		// else
		// {
		// 	const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, DTExportFlags);
		// 	JsonWriter->WriteValue(Identifier, PropertyValue);
		// }
	}
	else
	{
		// const FString PropertyValue = DataTableUtils::GetPropertyValueAsString(InProperty, (uint8*)InRowData, DTExportFlags);
		// JsonWriter->WriteValue(Identifier, PropertyValue);
	}

	return OutText;
}

const void* SUnrealDiffPropertyValueWidget::GetRowData(const FName& RowName)
{
	auto DataTable = OwnerTreeNode.Pin()->GetDetailsView()->GetDataTable();
	if (DataTable)
	{
		for (const auto& RowIt : DataTable->GetRowMap())
		{
			if (RowIt.Key.IsEqual(RowName))
			{
				return RowIt.Value;
			}
		}
	}

	return nullptr;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
