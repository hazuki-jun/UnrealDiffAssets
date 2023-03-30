#pragma once
#include "UnrealDiffDetailTreeNode.h"

class FUnrealDiffCategoryItemNode : public FUnrealDiffDetailTreeNode, public TSharedFromThis<FUnrealDiffCategoryItemNode>
{
public:
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) override;
};
