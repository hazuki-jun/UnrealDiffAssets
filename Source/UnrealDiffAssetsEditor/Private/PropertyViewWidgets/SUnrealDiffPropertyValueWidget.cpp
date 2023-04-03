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
	const FProperty* Property = OwnerTreeNode.Pin().Get()->PropertyData->Property.Get();
	FText Value = GetValueText(Property);
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
		// else if (auto FloatProperty = CastField<FFloatProperty>(Property))
		// {
			// FloatProperty->GetPropertyValue()
			// FString ValueStr;
			// const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData;
			// if (!StructData)
			// {
			// 	return;
			// }
			//
			// Property->ExportText_InContainer(0, ValueStr, StructData, StructData, NULL, PPF_None);
			// Value = DataTableUtils::GetPropertyValueAsText(Property, StructData);	
		// }
		else
		{
			if (const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData)
			{
				const void* Data = Property->ContainerPtrToValuePtr<void>(StructData, 0);
				
				// Value = DataTableUtils::GetPropertyValueAsText(Property, StructData);
			}
			else
			{
				
				if (auto StructOnScope = OwnerTreeNode.Pin().Get()->PropertyData->StructOnScope)
				{
					for (TPropertyValueIterator<const FProperty> It(StructOnScope->GetStruct(), StructOnScope->GetStructMemory()); It; ++It)
					{
						if (Property == It.Key())
						{
							const void* PropertyValue = It.Value();
						}
					}	
				}
			}
			
			// if (const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData)
			// {
			// 	Value = DataTableUtils::GetPropertyValueAsText(Property, StructData);
			// }
			// else if (OwnerTreeNode.Pin().Get()->PropertyData->StructOnScope.IsValid())
			// {
			// 	auto Parnet = OwnerTreeNode.Pin().Get()->PropertyData->StructOnScope->GetStruct();
			// 	
			// 	auto StructMemory = OwnerTreeNode.Pin().Get()->PropertyData->StructOnScope->GetStructMemory();
			// 	if (StructMemory && Parnet)
			// 	{
			// 		if(auto ValueAddress = Property->ContainerPtrToValuePtr<uint8>(StructMemory, 0))
			// 		{
			// 			if (*ValueAddress != '\0')
			// 			{
			// 				FString ValueStr;
			// 				Property->ExportText_InContainer(0, ValueStr, Parnet, Parnet, NULL, PPF_None);
			// 				Value = FText::FromString(ValueStr);	
			// 			}
			// 		}
			// 	}
			// }
		}
		// else if (auto IntProperty = CastField<FIntProperty>(Property))
		// {
		// 	const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData;
		// 	if (!StructData)
		// 	{
		// 		return;
		// 	}
		// 	int32 IntValue;
		// 	IntProperty->GetValue_InContainer(StructData, &IntValue);
		// 	Value = FText::FromString(FString::FromInt(IntValue));
		// }
		// else if (auto StrProperty = CastField<FStrProperty>(Property))
		// {
		// 	const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData;
		// 	if (!StructData)
		// 	{
		// 		return;
		// 	}
		// 	FString StrValue;
		// 	StrProperty->GetValue_InContainer(StructData, &StrValue);
		// 	Value = FText::FromString(StrValue);
		// }
		// else if (auto NameProperty = CastField<FNameProperty>(Property))
		// {
		// 	const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData;
		// 	if (!StructData)
		// 	{
		// 		return;
		// 	}
		// 	FName NameValue;
		// 	NameProperty->GetValue_InContainer(StructData, &NameValue);
		// 	Value = FText::FromName(NameValue);
		// }
		// else if (auto TextProperty = CastField<FTextProperty>(Property))
		// {
		// 	FString ValueStr;
		// 	const auto StructData = OwnerTreeNode.Pin().Get()->PropertyData->StructData;
		// 	Property->ExportText_InContainer(0, ValueStr, StructData, StructData, NULL, PPF_None);
		// 	Value = DataTableUtils::GetPropertyValueAsText(Property, StructData);	
		// }
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

FText SUnrealDiffPropertyValueWidget::GetValueText(const FProperty* InProperty)
{
	FText OutText;
	const void* StructData = GetRowData(OwnerTreeNode.Pin()->GetDetailsView()->GetCurrentRowName());
	
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
			if (OwnerTreeNode.Pin()->PropertyData->ParentStructProperty.Get())
			{
				auto ParentStructData = OwnerTreeNode.Pin()->PropertyData->ParentStructProperty->ContainerPtrToValuePtr<void>(StructData);
				auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(ParentStructData, 0);
				const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
				OutText = FText::FromString(FString::FromInt(PropertyValue));
			}
			else
			{
				auto ValueAddress =  NumProp->ContainerPtrToValuePtr<void>(StructData, 0);
				const int64 PropertyValue = NumProp->GetSignedIntPropertyValue(ValueAddress);
				OutText = FText::FromString(FString::FromInt(PropertyValue));
			}
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
