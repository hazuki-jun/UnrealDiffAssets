#include "UnrealDiffDetailItemNode.h"

#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

TSharedRef<ITableRow> FUnrealDiffDetailItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailSingleItemRow);
}

void FUnrealDiffDetailItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	
}
