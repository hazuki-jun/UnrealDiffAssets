#include "UnrealDiffDetailTreeNode.h"

FUnrealDiffDetailTreeNode::FUnrealDiffDetailTreeNode()
{
}

FUnrealDiffDetailTreeNode::~FUnrealDiffDetailTreeNode()
{
}

FText FUnrealDiffDetailTreeNode::GetValueText()
{
	
	return FText();
}

FString FUnrealDiffDetailTreeNode::GetUniqueNodeId() const
{
	return NodeId + GetParentUniqueNodeId();
}

FString FUnrealDiffDetailTreeNode::GetParentUniqueNodeId() const
{
	return FString();
}

FName FUnrealDiffDetailTreeNode::GetCategoryName()
{
	if (Property.IsValid() && IsContainerNode())
	{
		return Property->GetFName();
	}

	return NAME_None;
}

bool FUnrealDiffDetailTreeNode::HasAnyDifferenceRecurse()
{
	bool Ret = bHasAnyDifference;
	if (Children.Num() > 0)
	{
		for (const auto& TreeNode : Children)
		{
			if (TreeNode->HasAnyDifferenceRecurse())
			{
				Ret = true;
			}
		}		
	}
	
	return Ret;
}

const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& FUnrealDiffDetailTreeNode::GetChildNodes()
{
	return Children;
}

