#pragma once
#include "UnrealDiffDetailTreeNode.h"

class FUnrealDiffDetailItemNode : public FUnrealDiffDetailTreeNode, public TSharedFromThis<FUnrealDiffDetailItemNode>
{
public:
	FUnrealDiffDetailItemNode(const FProperty* InProperty, class SUnrealDiffDetailView* DetailView_)
	: Property(InProperty)
	, DetailView(DetailView_)
	{}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) override;

	virtual const FProperty* GetProperty() const override;

	virtual class SUnrealDiffDetailView* GetDetailsView() const override;
	
	TWeakFieldPtr<const FProperty> Property;

	class SUnrealDiffDetailView* DetailView;
};
