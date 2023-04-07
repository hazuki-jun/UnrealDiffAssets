#include "UnrealDiffDetailTreeNode.h"

FUnrealDiffDetailTreeNode::FUnrealDiffDetailTreeNode()
{
}

FUnrealDiffDetailTreeNode::~FUnrealDiffDetailTreeNode()
{
}

FName FUnrealDiffDetailTreeNode::GetCategoryName()
{
	if (Property.IsValid() && IsContainerNode())
	{
		return Property->GetFName();
	}

	return NAME_None;
}

const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& FUnrealDiffDetailTreeNode::GetChildNodes()
{
	return Children;
}

