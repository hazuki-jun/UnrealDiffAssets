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
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
	else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		if (StructData)
		{
			FScriptMapHelper MapHelper(MapProp, StructData);
			for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
			{
				if (MapHelper.IsValidIndex(MapSparseIndex))
				{
					// TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
					// if (DetailView)
					// {
					// 	DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
					// 	DetailView->AddCacheNode(DetailItemNode);
					// }
					// DetailItemNode->Property = MapProp;
					// DetailItemNode->PropertyIndex = MapSparseIndex;
					// DetailItemNode->ParentNode = AsShared();
					// DetailItemNode->GenerateChildren();
					// Children.Add(DetailItemNode);
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
				DetailItemNode->ParentNode = AsShared();
				DetailItemNode->RowDataInContainer = SetHelper.GetElementPtr(SetSparseIndex);
				DetailItemNode->GenerateChildren();
				Children.Add(DetailItemNode);
				// const uint8* SetEntryData = SetHelper.GetElementPtr(SetSparseIndex);
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
			DetailItemNode->RowDataInContainer = ArrayHelper.GetRawPtr(ArrayEntryIndex);
			DetailItemNode->ParentNode = AsShared();
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
}

const void* FUnrealDiffDetailItemNode::GetStructData(int32 ArrayIndex)
{
	if (ParentNode.IsValid())
	{
		if (bIsInContainer)
		{
			return RowDataInContainer;
		}
		
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

