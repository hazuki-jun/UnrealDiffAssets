#include "UnrealDiffDetailTreeNode.h"

FUnrealDiffDetailTreeNode::FUnrealDiffDetailTreeNode()
{
}

FUnrealDiffDetailTreeNode::~FUnrealDiffDetailTreeNode()
{
}

const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& FUnrealDiffDetailTreeNode::GetChildNodes()
{
	return Children;
}

