#pragma once

#include "UnrealDiffDetailTreeNode.h"

class FUnrealDiffCategoryItemNode : public FUnrealDiffDetailTreeNode, public TSharedFromThis<FUnrealDiffCategoryItemNode>
{
public:
	FUnrealDiffCategoryItemNode(FName CategoryName_, class SUnrealDiffDetailView* DetailView_)
	: CategoryName(CategoryName_)
	, DetailView(DetailView_)
	{}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) override;

	virtual class SUnrealDiffDetailView* GetDetailsView() const override;

	virtual void GenerateChildren();

	virtual const void* GetStructData(int32 ArrayIndex = 0) override;
	
	FName CategoryName;

	class SUnrealDiffDetailView* DetailView;
	
	TArray<TWeakFieldPtr<const class FProperty>> ChildPropertyArray;
};
