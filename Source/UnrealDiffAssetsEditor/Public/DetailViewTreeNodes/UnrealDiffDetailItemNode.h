#pragma once

#include "UnrealDiffDetailTreeNode.h"

class FUnrealDiffDetailItemNode : public FUnrealDiffDetailTreeNode, public TSharedFromThis<FUnrealDiffDetailItemNode>
{
public:
	FUnrealDiffDetailItemNode(class SUnrealDiffDetailView* DetailView_)
	: DetailView(DetailView_)
	{}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) override;

	virtual class SUnrealDiffDetailView* GetDetailsView() const override;

	virtual void GenerateChildren();

	virtual const void* GetStructData(int32 ArrayIndex = 0) override;

	TWeakPtr<FUnrealDiffDetailTreeNode> ParentNode;
	
	class SUnrealDiffDetailView* DetailView;

protected:
	const void* GetContainerData(const void* InStructData);
};
