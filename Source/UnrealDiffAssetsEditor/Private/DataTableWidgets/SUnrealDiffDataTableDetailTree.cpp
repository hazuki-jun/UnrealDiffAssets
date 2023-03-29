// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"

#include "SlateOptMacros.h"
#include "DataTableWidgets/DetailViewTreeNodes/UnrealDataTableDetailTreeNode.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableDetailTree::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(STreeView<TSharedPtr<UnrealDataTableDetailTreeNode>>)
		.TreeItemsSource(&RootTreeNodes)
		.OnGenerateRow(this, &SUnrealDiffDataTableDetailTree::OnGenerateRowForDetailTree)
	];
}

TSharedRef<ITableRow> SUnrealDiffDataTableDetailTree::OnGenerateRowForDetailTree(TSharedPtr<UnrealDataTableDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return InTreeNode->GenerateWidgetForTableView(OwnerTable);
}

void SUnrealDiffDataTableDetailTree::SetStructure(TSharedPtr<FStructOnScope> CurrentRow)
{
	
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
