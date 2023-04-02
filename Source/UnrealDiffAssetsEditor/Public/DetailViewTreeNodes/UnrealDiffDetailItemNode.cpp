#include "UnrealDiffDetailItemNode.h"

#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

TSharedRef<ITableRow> FUnrealDiffDetailItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailSingleItemRow, OwnerTable, AsShared());
}

void FUnrealDiffDetailItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	for (const auto Property : ChildPropertyArray)
	{
		TSharedPtr<FUnrealDiffDetailItemNode> CategoryNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		CategoryNode->PropertyData = Property;
		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property->Property.Get()))
		{
			// return MakeShareable(new FStructOnScope(StructProp->Struct));
		}
		
		OutChildren.Add(CategoryNode);
	}
}

SUnrealDiffDetailView* FUnrealDiffDetailItemNode::GetDetailsView() const
{
	return DetailView;
}
