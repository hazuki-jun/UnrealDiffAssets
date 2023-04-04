// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailView::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}


void SUnrealDiffDetailView::AppendCacheNodes(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> InNodes)
{
	AllNodes.Append(InNodes);
}

void SUnrealDiffDetailView::AddCacheNode(TSharedPtr<FUnrealDiffDetailTreeNode> InNode)
{
	AllNodes.Add(InNode);
}

TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> SUnrealDiffDetailView::GetCachedNodes() const
{
	return AllNodes;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
