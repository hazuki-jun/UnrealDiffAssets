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

	virtual void* GetStructData(int32 ArrayIndex = 0) { return nullptr; }

	bool IsContainerNode() const { return Children.Num() > 0; }
	
	TWeakFieldPtr<const class FProperty> Property;
	
	bool bIsInContainer = false;
	bool bIsMapValue = false;
	bool bIsMapKey = false;
	bool bIsMapCategory = false;
	
	TWeakFieldPtr<const class FProperty> ContainerProperty;

	uint8* RowDataInContainer = nullptr;
	
	int32 PropertyIndex = -1;

	int32 GetNodeIndex() const { return NodeIndex; }
	
	void SetNodeIndex(int32 InNodeIndex) { NodeIndex = InNodeIndex; }
	
	FString GetUniqueNodeId() const;

	virtual FString GetParentUniqueNodeId() const;
	
	void SetNodeId(const FString& InUniqueNodeId) { NodeId = InUniqueNodeId; }
	
	virtual FName GetCategoryName();
	
	FText ValueText;

	bool HasAnyDifferenceRecurse();
	
	bool bHasAnyDifference = false;

	const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& GetChildNodes();
	
protected:
	TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> Children;

	int32 NodeIndex = -1;

	FString NodeId;
};
