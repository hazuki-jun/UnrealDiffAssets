#include "UnrealDiffCategoryItemNode.h"

#include "UnrealDiffDetailItemNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

TSharedRef<ITableRow> FUnrealDiffCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailCategoryRow, OwnerTable).CategoryName(CategoryName);
}

void FUnrealDiffCategoryItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	for (const FProperty* Property : ChildPropertyArray)
	{
		const FStructProperty* ParentStructProp = CastField<FStructProperty>(Property);
		// if (ParentStructProp)
		// {
		// 	TSharedPtr<FUnrealDiffCategoryItemNode> CategoryNode = MakeShareable(new FUnrealDiffCategoryItemNode(ParentStructProp->GetFName()));
		// 	OutChildren.Add(CategoryNode);
		// }
		// else
		{
			TSharedPtr<FUnrealDiffDetailItemNode> CategoryNode = MakeShareable(new FUnrealDiffDetailItemNode(Property, DetailView));
			OutChildren.Add(CategoryNode);
		}
	}
}

SUnrealDiffDetailView* FUnrealDiffCategoryItemNode::GetDetailsView() const
{
	return DetailView;
}
