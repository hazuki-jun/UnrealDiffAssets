#pragma once
#include "UnrealDiffDetailTreeNode.h"

class FUnrealDiffCategoryItemNode : public FUnrealDiffDetailTreeNode, public TSharedFromThis<FUnrealDiffCategoryItemNode>
{
public:
	FUnrealDiffCategoryItemNode(FName CategoryName_) : CategoryName(CategoryName_) {}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) override;

	FName CategoryName;
};
