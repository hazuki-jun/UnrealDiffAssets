#pragma once

#include "CoreMinimal.h"

class FUnrealDiffDetailTreeNode
{
public:
	FUnrealDiffDetailTreeNode();
	virtual ~FUnrealDiffDetailTreeNode();

	/**
	 * Generates the widget representing this node
	 *
	 * @param OwnerTable		The table owner of the widget being generated
	 */
	virtual TSharedRef< ITableRow > GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) = 0;

	/**
	 * Gets child tree nodes
	 *
	 * @param OutChildren [out]	The array to add children to
	 */
	virtual void GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren) = 0;

	/** @return The details view that this node is in */
	virtual class SUnrealDiffDetailView* GetDetailsView() const = 0;

	virtual const void* GetStructData(int32 ArrayIndex = 0) { return nullptr; }
	
	TWeakFieldPtr<const class FProperty> Property;
	
protected:
	TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> Children;
};
