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
	// for (auto Child : Children)
	// {
	// 	OutChildren.Add(Child);
	// }
	OutChildren = Children;
}

SUnrealDiffDetailView* FUnrealDiffDetailItemNode::GetDetailsView() const
{
	return DetailView;
}

FText FUnrealDiffDetailItemNode::GetDisplayNameText()
{
	return DisplayNameText; 
}

FString FUnrealDiffDetailItemNode::GetPropertyValueAsString()
{
	return ValueText.ToString();
}

FText FUnrealDiffDetailItemNode::GetValueText()
{
	return ValueText;
}

void FUnrealDiffDetailItemNode::GenerateChildren()
{
	auto CreateChildItemNode = [this](TWeakFieldPtr<const class FProperty> InProperty, bool bAddToCache)
	{
		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		if (bAddToCache)
		{
			DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());	
			DetailView->AddCacheNode(DetailItemNode);
		}
		
		DetailItemNode->ParentNode = AsShared();
		DetailItemNode->Property = InProperty;

		return DetailItemNode;
	};
	
	if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
	{
		for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
		{
			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = CreateChildItemNode(*It, true);
			DetailItemNode->SetNodeId(*It->GetName());
			DetailItemNode->ValueText = GetValueTextStructInternal(*It);
			DetailItemNode->GenerateChildren();
			DetailItemNode->DisplayNameText = It->GetDisplayNameText();
			Children.Add(DetailItemNode);
		}
	}
	else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		FScriptArrayHelper ArrayHelper(ArrayProp, StructData);
		for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < ArrayHelper.Num(); ++ArrayEntryIndex)
		{
			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = CreateChildItemNode(ArrayProp->Inner, true);
			DetailItemNode->RawPtr = ArrayHelper.GetRawPtr(ArrayEntryIndex);
			DetailItemNode->ContainerIndex = ArrayEntryIndex;
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(ArrayEntryIndex));
			DetailItemNode->ValueText = GetValueTextInternal(DetailItemNode->RawPtr, ArrayProp->Inner);
			DetailItemNode->GenerateChildren();
			DetailItemNode->DisplayNameText = FText::FromString(FString::Format(TEXT("[{0}]"), {ArrayEntryIndex}));
			Children.Add(DetailItemNode);
		}
	}
	else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		FScriptSetHelper SetHelper(SetProp, StructData);
		for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
		{
			if (!SetHelper.IsValidIndex(SetSparseIndex))
			{
				return;
			}

			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = CreateChildItemNode(SetHelper.GetElementProperty(), true);
			DetailItemNode->RawPtr = SetHelper.GetElementPtr(SetSparseIndex);
			DetailItemNode->ContainerIndex = SetSparseIndex;
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(SetSparseIndex));
			DetailItemNode->ValueText = GetValueTextInternal(DetailItemNode->RawPtr, SetHelper.GetElementProperty());
			DetailItemNode->GenerateChildren();
			DetailItemNode->DisplayNameText = FText::FromString(FString::Format(TEXT("[{0}]"), {SetSparseIndex}));
			Children.Add(DetailItemNode);
		}
	}
	
	// else if (const FMapProperty* StructProp = CastField<FStructProperty>(Property.Get()))
	// {
	// }
#pragma region Disable
	// else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
	// {
	// 	auto StructData = GetStructData(0);
	// 	if (!StructData)
	// 	{
	// 		return;
	// 	}
	//
	// 	FScriptSetHelper SetHelper(SetProp, StructData);
	// 	for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
	// 	{
	// 		if (SetHelper.IsValidIndex(SetSparseIndex))
	// 		{
	// 			TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
	// 			if (DetailView)
	// 			{
	// 				DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
	// 				DetailView->AddCacheNode(DetailItemNode);
	// 			}
	// 			DetailItemNode->bIsInContainer = true;
	// 			DetailItemNode->ContainerProperty = SetProp;
	// 			DetailItemNode->PropertyIndex = SetSparseIndex;
	// 			DetailItemNode->Property = SetHelper.GetElementProperty();
	// 			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(SetSparseIndex));
	// 			DetailItemNode->ParentNode = AsShared();
	// 			DetailItemNode->RowDataInContainer = SetHelper.GetElementPtr(SetSparseIndex);
	// 			if (DetailItemNode->RowDataInContainer)
	// 			{
	// 				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(DetailItemNode->Property.Get(), DetailItemNode->RowDataInContainer);
	// 			}
	// 			DetailItemNode->GenerateChildren();
	// 			Children.Add(DetailItemNode);
	// 		}
	// 	}
	// }
	// else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	// {
	// 	auto StructData = GetStructData(0);
	// 	if (StructData)
	// 	{
	// 		void* RowData = MapProp->ContainerPtrToValuePtr<void>(StructData);
	// 		FScriptMapHelper MapHelper(MapProp, RowData);
	// 		for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
	// 		{
	// 			if (MapHelper.IsValidIndex(MapSparseIndex))
	// 			{
	// 				TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
	// 				// if (DetailView)
	// 				// {
	// 				// 	DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
	// 				// 	DetailView->AddCacheNode(DetailItemNode);
	// 				// }
	// 				DetailItemNode->bIsMapCategory = true;
	// 				DetailItemNode->Property = MapProp;
	// 				DetailItemNode->RowDataInContainer = (uint8*)RowData;
	// 				DetailItemNode->PropertyIndex = MapSparseIndex;
	// 				DetailItemNode->ParentNode = AsShared();
	// 				DetailItemNode->GenerateChildren();
	// 				Children.Add(DetailItemNode);
	// 			}
	// 		}
	// 	}
	// }
#pragma endregion 
}

void FUnrealDiffDetailItemNode::GenerateMapKeyValueChildren()
{
	// if (!RowDataInContainer)
	// {
	// 	return;
	// }
	//
	// if (auto MapProp = CastField<FMapProperty>(Property.Get()))
	// {
	// 	FScriptMapHelper MapHelper(MapProp, RowDataInContainer);
	// 	if (!MapHelper.IsValidIndex(ContainerIndex))
	// 	{
	// 		return;
	// 	}
	// 	
	// 	TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNodeKey = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
	// 	if (DetailView)
	// 	{
	// 		DetailItemNodeKey->SetNodeIndex(DetailView->GetCachedNodeNum());
	// 		DetailView->AddCacheNode(DetailItemNodeKey);
	// 	}
	// 	DetailItemNodeKey->Property = MapProp->GetKeyProperty();
	// 	DetailItemNodeKey->RowDataInContainer = MapHelper.GetKeyPtr(ContainerIndex);
	// 	DetailItemNodeKey->ParentNode = AsShared();
	// 	DetailItemNodeKey->bIsInContainer = true;
	// 	DetailItemNodeKey->bIsMapKey = true;
	// 	if (DetailItemNodeKey->RowDataInContainer)
	// 	{
	// 		DetailItemNodeKey->ValueText = DataTableUtils::GetPropertyValueAsTextDirect(MapHelper.GetKeyProperty(), DetailItemNodeKey->RowDataInContainer);
	// 	}
	// 	DetailItemNodeKey->GenerateChildren();
	// 	Children.Add(DetailItemNodeKey);
	//
	// 	TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNodeValue = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
	// 	if (DetailView)
	// 	{
	// 		DetailItemNodeValue->SetNodeIndex(DetailView->GetCachedNodeNum());
	// 		DetailView->AddCacheNode(DetailItemNodeValue);
	// 	}
	// 	DetailItemNodeValue->Property = MapProp->GetValueProperty();
	// 	DetailItemNodeValue->RowDataInContainer = MapHelper.GetValuePtr(ContainerIndex);
	// 	DetailItemNodeValue->ParentNode = AsShared();
	// 	DetailItemNodeValue->bIsMapValue = true;
	// 	DetailItemNodeValue->bIsInContainer = true;
	// 	if (DetailItemNodeValue->RowDataInContainer)
	// 	{
	// 		DetailItemNodeValue->ValueText = GetValueTextEntry(MapHelper.GetValueProperty(), DetailItemNodeValue->RowDataInContainer);
	// 	}
	// 	DetailItemNodeValue->GenerateChildren();
	// 	Children.Add(DetailItemNodeValue);
	// }
}

void* FUnrealDiffDetailItemNode::GetStructData(int32 ArrayIndex)
{
	if (IsInContainer())
	{
		return RawPtr;
	}
	
	if (ParentNode.IsValid())
	{
		auto ParentStructData = ParentNode.Pin()->GetStructData();

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

FString FUnrealDiffDetailItemNode::GetParentUniqueNodeId() const
{
	if (ParentNode.IsValid())
	{
		return ParentNode.Pin()->GetUniqueNodeId();
	}

	return FString();
}

