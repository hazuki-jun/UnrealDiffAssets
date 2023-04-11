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

	bool IsContainerNode() const;

	bool IsInContainer() const { return ContainerIndex >= 0; }

	bool IsMapKeyOrValue();
	
	TWeakFieldPtr<const class FProperty> Property;
	
	uint8* RawPtr = nullptr;
	
	int32 ContainerIndex = -1;

	int32 GetNodeIndex() const { return NodeIndex; }

	virtual FString GetPropertyValueAsString();
	
	virtual FText GetValueText();

	virtual FText GetDisplayNameText();
	
	void SetNodeIndex(int32 InNodeIndex) { NodeIndex = InNodeIndex; }
	
	FString GetUniqueNodeId() const;

	virtual FString GetParentUniqueNodeId() const;
	
	void SetNodeId(const FString& InUniqueNodeId) { NodeId = InUniqueNodeId; }
	
	virtual FName GetCategoryName();
	
	bool HasAnyDifferenceRecurse();
	
	const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& GetChildNodes();

protected:
	virtual FText GetValueTextStructInternal(const FProperty* InProperty);
	virtual FText GetValueTextContainerInternal(void* PropertyData, const FProperty* InProperty);
	virtual FText GetValueTextInternal(void* ValueAddress, const FProperty* InProperty);

public:
	FText DisplayNameText;
	
	bool bHasAnyDifference = false;
	
	FText ValueText;
	
	bool bIsExpanded = false;
	
protected:
	TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> Children;

	int32 NodeIndex = -1;

	FString NodeId;
};
