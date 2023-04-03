#include "UnrealDiffDetailItemNode.h"

#include "ObjectEditorUtils.h"
#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

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
			DetailItemNode->Property = *It;
			DetailItemNode->GenerateChildren();
			DetailItemNode->ParentNode = AsShared();
			Children.Add(DetailItemNode);
		}
	}
}

const void* FUnrealDiffDetailItemNode::GetStructData(int32 ArrayIndex)
{
	if (ParentNode.IsValid())
	{
		auto ParentStructData = ParentNode.Pin()->GetStructData();
		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
		{
			if (ParentStructData)
			{
				return StructProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
			}
		}
		return ParentStructData;
	}
	
	return nullptr;
}

