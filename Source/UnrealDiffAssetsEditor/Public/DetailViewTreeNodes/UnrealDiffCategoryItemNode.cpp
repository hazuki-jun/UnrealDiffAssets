#include "UnrealDiffCategoryItemNode.h"

#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

TSharedRef<ITableRow> FUnrealDiffCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailCategoryRow, OwnerTable).CategoryName(CategoryName);
}

void FUnrealDiffCategoryItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
}
