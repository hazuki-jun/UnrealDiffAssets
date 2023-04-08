#include "UnrealDiffDetailItemNode.h"

#include "ObjectEditorUtils.h"
#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

TSharedRef<ITableRow> FUnrealDiffDetailItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailSingleItemRow, OwnerTable, AsShared());
}

void FUnrealDiffDetailItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	for (auto Child : Children)
	{
		OutChildren.Add(Child);
	}
}

SUnrealDiffDetailView* FUnrealDiffDetailItemNode::GetDetailsView() const
{
	return DetailView;
}

void FUnrealDiffDetailItemNode::GenerateChildren()
{
	if (bIsMapCategory)
	{
		GenerateMapKeyValueChildren();
		return;
	}
	
	if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
	{
		for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
		{
			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
			if (DetailView)
			{
				DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
				DetailView->AddCacheNode(DetailItemNode);
			}
			DetailItemNode->ParentNode = AsShared();
			DetailItemNode->Property = *It;
			DetailItemNode->SetNodeId(*It->GetName());
			DetailItemNode->GenerateChildren();
			if (const auto StructData = GetStructData(0))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(*It, (const uint8*)StructData);
			}
			Children.Add(DetailItemNode);
		}
	}
	else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		if (StructData)
		{
			void* RowData = MapProp->ContainerPtrToValuePtr<void>(StructData);
			FScriptMapHelper MapHelper(MapProp, RowData);
			for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
			{
				if (MapHelper.IsValidIndex(MapSparseIndex))
				{
					TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
					// if (DetailView)
					// {
					// 	DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
					// 	DetailView->AddCacheNode(DetailItemNode);
					// }
					DetailItemNode->bIsMapCategory = true;
					DetailItemNode->Property = MapProp;
					DetailItemNode->RowDataInContainer = (uint8*)RowData;
					DetailItemNode->PropertyIndex = MapSparseIndex;
					DetailItemNode->ParentNode = AsShared();
					DetailItemNode->GenerateChildren();
					Children.Add(DetailItemNode);
				}
			}
		}
	}
	else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		if (!StructData)
		{
			return;
		}

		FScriptSetHelper SetHelper(SetProp, StructData);
		for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
		{
			if (SetHelper.IsValidIndex(SetSparseIndex))
			{
				TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
				if (DetailView)
				{
					DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
					DetailView->AddCacheNode(DetailItemNode);
				}
				DetailItemNode->bIsInContainer = true;
				DetailItemNode->ContainerProperty = SetProp;
				DetailItemNode->PropertyIndex = SetSparseIndex;
				DetailItemNode->Property = SetHelper.GetElementProperty();
				DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(SetSparseIndex));
				DetailItemNode->ParentNode = AsShared();
				DetailItemNode->RowDataInContainer = SetHelper.GetElementPtr(SetSparseIndex);
				if (DetailItemNode->RowDataInContainer)
				{
					DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(DetailItemNode->Property.Get(), DetailItemNode->RowDataInContainer);
				}
				DetailItemNode->GenerateChildren();
				Children.Add(DetailItemNode);
			}
		}
	}
	else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		if (!StructData)
		{
			return;
		}
		FScriptArrayHelper ArrayHelper(ArrayProp, StructData);
		for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < ArrayHelper.Num(); ++ArrayEntryIndex)
		{
			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
			if (DetailView)
			{
				DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
				DetailView->AddCacheNode(DetailItemNode);
			}
			DetailItemNode->bIsInContainer = true;
			DetailItemNode->ContainerProperty = ArrayProp;
			DetailItemNode->PropertyIndex = ArrayEntryIndex;
			DetailItemNode->Property = ArrayProp->Inner;
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(ArrayEntryIndex));
			DetailItemNode->RowDataInContainer = ArrayHelper.GetRawPtr(ArrayEntryIndex);
			if (DetailItemNode->RowDataInContainer)
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(DetailItemNode->Property.Get(), DetailItemNode->RowDataInContainer);
			}
			DetailItemNode->ParentNode = AsShared();
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
}

void FUnrealDiffDetailItemNode::GenerateMapKeyValueChildren()
{
	if (!RowDataInContainer)
	{
		return;
	}

	if (auto MapProp = CastField<FMapProperty>(Property.Get()))
	{
		FScriptMapHelper MapHelper(MapProp, RowDataInContainer);
		if (!MapHelper.IsValidIndex(PropertyIndex))
		{
			return;
		}
		
		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNodeKey = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		if (DetailView)
		{
			DetailItemNodeKey->SetNodeIndex(DetailView->GetCachedNodeNum());
			DetailView->AddCacheNode(DetailItemNodeKey);
		}
		DetailItemNodeKey->Property = MapProp->GetKeyProperty();
		DetailItemNodeKey->RowDataInContainer = MapHelper.GetKeyPtr(PropertyIndex);
		DetailItemNodeKey->ParentNode = AsShared();
		DetailItemNodeKey->bIsInContainer = true;
		DetailItemNodeKey->bIsMapKey = true;
		if (DetailItemNodeKey->RowDataInContainer)
		{
			DetailItemNodeKey->ValueText = DataTableUtils::GetPropertyValueAsTextDirect(MapHelper.GetKeyProperty(), DetailItemNodeKey->RowDataInContainer);
		}
		DetailItemNodeKey->GenerateChildren();
		Children.Add(DetailItemNodeKey);

		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNodeValue = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		if (DetailView)
		{
			DetailItemNodeValue->SetNodeIndex(DetailView->GetCachedNodeNum());
			DetailView->AddCacheNode(DetailItemNodeValue);
		}
		DetailItemNodeValue->Property = MapProp->GetValueProperty();
		DetailItemNodeValue->RowDataInContainer = MapHelper.GetValuePtr(PropertyIndex);
		DetailItemNodeValue->ParentNode = AsShared();
		DetailItemNodeValue->bIsMapValue = true;
		DetailItemNodeValue->bIsInContainer = true;
		if (DetailItemNodeValue->RowDataInContainer)
		{
			DetailItemNodeValue->ValueText = GetValueTextEntry(MapHelper.GetValueProperty(), DetailItemNodeValue->RowDataInContainer);
		}
		DetailItemNodeValue->GenerateChildren();
		Children.Add(DetailItemNodeValue);
	}
}

void* FUnrealDiffDetailItemNode::GetStructData(int32 ArrayIndex)
{
	if (ParentNode.IsValid())
	{
		if (bIsInContainer)
		{
			return RowDataInContainer;
		}
		
		auto ParentStructData = ParentNode.Pin()->GetStructData();
		check(ParentStructData)
		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
		{
			return StructProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
		}
		else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
		{
			if (ParentStructData)
			{
				return ArrayProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
			}
		}
		else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
		{
			if (ParentStructData)
			{
				return SetProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
			}
		}
		return ParentStructData;
	}
	
	return nullptr;
}

FText FUnrealDiffDetailItemNode::GetValueTextEntry(FProperty* InProperty, void* Data)
{
	if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
	{
		auto ValueAddress = InProperty->ContainerPtrToValuePtr<void>(Data);
	}

	return FText();
}

FString FUnrealDiffDetailItemNode::GetParentUniqueNodeId() const
{
	if (ParentNode.IsValid())
	{
		return ParentNode.Pin()->GetUniqueNodeId();
	}

	return FString();
}

const void* FUnrealDiffDetailItemNode::GetContainerData(const void* InStructData)
{
	if (InStructData)
	{
		if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
		{
			if (InStructData)
			{
				return ArrayProp->ContainerPtrToValuePtr<void>(InStructData, 0);
			}
		}
		else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
		{
			if (InStructData)
			{
				return SetProp->ContainerPtrToValuePtr<void>(InStructData, 0);
			}
		}
	}

	return nullptr;
}

