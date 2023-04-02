#include "UnrealDiffCategoryItemNode.h"

#include "UnrealDiffDetailItemNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

TSharedRef<ITableRow> FUnrealDiffCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailCategoryRow, OwnerTable).CategoryName(CategoryName);
}

void FUnrealDiffCategoryItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
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

SUnrealDiffDetailView* FUnrealDiffCategoryItemNode::GetDetailsView() const
{
	return DetailView;
}
