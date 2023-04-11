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

	virtual FText GetDisplayNameText() override;

	virtual FString GetPropertyValueAsString() override;
	
	virtual FText GetValueText() override;
	
	virtual void GenerateChildren();

	void GenerateMapKeyValueChildren();
	
	virtual void* GetStructData(int32 ArrayIndex = 0) override;
	
	virtual FString GetParentUniqueNodeId() const override;
	
	TWeakPtr<FUnrealDiffDetailTreeNode> ParentNode;
	
	class SUnrealDiffDetailView* DetailView;

	bool bIsExpanded = false;
};


