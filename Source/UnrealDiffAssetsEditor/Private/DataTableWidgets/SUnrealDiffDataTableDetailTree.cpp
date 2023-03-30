// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"

#include "ObjectEditorUtils.h"
#include "SlateOptMacros.h"
#include "DetailViewTreeNodes/UnrealDiffCategoryItemNode.h"
#include "DetailViewTreeNodes/UnrealDiffDetailItemNode.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableDetailTree::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(MyTreeView, STreeView<TSharedPtr<FUnrealDiffDetailTreeNode>>)
		//~ Begin TreeView Interface
		.TreeItemsSource(&TreeNodes)
		.OnGenerateRow(this, &SUnrealDiffDataTableDetailTree::OnGenerateRowForDetailTree)
		.OnGetChildren(this, &SUnrealDiffDataTableDetailTree::OnGetChildrenForDetailTree)
		//~ End TreeView Interface
	];
}

TSharedRef<ITableRow> SUnrealDiffDataTableDetailTree::OnGenerateRowForDetailTree(TSharedPtr<FUnrealDiffDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return InTreeNode->GenerateWidgetForTableView(OwnerTable);
}

void SUnrealDiffDataTableDetailTree::OnGetChildrenForDetailTree(TSharedPtr<FUnrealDiffDetailTreeNode> InTreeNode, TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	InTreeNode->GetChildren(OutChildren);
}

void SUnrealDiffDataTableDetailTree::SetStructure(TSharedPtr<FStructOnScope> CurrentRow)
{
	if (!CurrentRow.IsValid())
	{
		return;
	}
	
	TreeNodes.Empty();
	TSet<FName> Categories;
	auto ScriptStruct = CurrentRow->GetStruct();
	if (ScriptStruct)
	{
		for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
		{
			FProperty* StructMember = *It;
			FName CategoryName =  FObjectEditorUtils::GetCategoryFName(StructMember);
			// TSharedPtr<FUnrealDiffDetailItemNode> NewNode = MakeShareable(new FUnrealDiffDetailItemNode());
			// TreeNodes.Add(NewNode);
			Categories.Add(CategoryName);
		}
	}

	for (const auto& Category : Categories)
	{
		TSharedPtr<FUnrealDiffCategoryItemNode> CategoryNode = MakeShareable(new FUnrealDiffCategoryItemNode(Category));
		TreeNodes.Add(CategoryNode);
	}
	
	MyTreeView->RequestTreeRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
