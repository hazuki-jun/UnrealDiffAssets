// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"

#include "ObjectEditorUtils.h"
#include "SlateOptMacros.h"
#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DetailViewTreeNodes/UnrealDiffCategoryItemNode.h"
#include "DetailViewTreeNodes/UnrealDiffDetailItemNode.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableDetailTree::Construct(const FArguments& InArgs, class SUnrealDiffDetailView* DetailView_)
{
	DetailView = DetailView_;
	
	ChildSlot
	[
		SAssignNew(MyTreeView, STreeView<TSharedPtr<FUnrealDiffDetailTreeNode>>)
		//~ Begin TreeView Interface
		.TreeItemsSource(&RootTreeNodes)
		.OnGenerateRow(this, &SUnrealDiffDataTableDetailTree::OnGenerateRowForDetailTree)
		.OnGetChildren(this, &SUnrealDiffDataTableDetailTree::OnGetChildrenForDetailTree)
		.OnExpansionChanged(this, &SUnrealDiffDataTableDetailTree::OnItemExpansionChanged)
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

void SUnrealDiffDataTableDetailTree::OnItemExpansionChanged(TSharedPtr<FUnrealDiffDetailTreeNode> TreeItem, bool bIsExpanded) const
{
	// TArray<TSharedPtr<FUnrealDiffDetailTreeNode>> Children;
	// TreeItem->GetChildren(Children);
	//
	// for (auto WeakChild : Children)
	// {
	// 	MyTreeView->SetItemExpansion(WeakChild, true);
	// }
}

void SUnrealDiffDataTableDetailTree::SetStructure(TSharedPtr<FUnrealDiffStructOnScope> Structure)
{
	if (!Structure.IsValid())
	{
		return;
	}

	TMap<FName, TArray<FProperty*>> StructMembers = GetStructMembers(Structure);
	RootTreeNodes.Empty();
	
	for (const auto& Category : StructMembers)
	{
		TSharedPtr<FUnrealDiffCategoryItemNode> CategoryNode = MakeShareable(new FUnrealDiffCategoryItemNode(Category.Key, DetailView));
		for (const auto Property : Category.Value)
		{
			TSharedPtr<FUnrealDiffPropertyData> PropertyData = MakeShareable(new FUnrealDiffPropertyData());
			PropertyData->StructData = GetRowData(Property);
			PropertyData->Property = Property;
			CategoryNode->ChildPropertyArray.Add(PropertyData);
		}
		
		CategoryNode->GenerateChildren();
		RootTreeNodes.Add(CategoryNode);
	}
	
	MyTreeView->RequestTreeRefresh();
}

TMap<FName, TArray<FProperty*>> SUnrealDiffDataTableDetailTree::GetStructMembers(TSharedPtr<FUnrealDiffStructOnScope> Structure)
{
	TMap<FName, TArray<FProperty*>> StructMembers;
	
	auto ScriptStruct = Structure->StructureData->GetStruct();
	if (ScriptStruct)
	{
		for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
		{
			FProperty* Property = *It;
			if (CastField<FObjectProperty>(Property))
			{
				continue;
			}
			
			FName CategoryName =  FObjectEditorUtils::GetCategoryFName(Property);
			if (CategoryName.IsNone())
			{
				CategoryName = Structure->CurrentRowName;
			}
			auto& Found = StructMembers.FindOrAdd(CategoryName);
			Found.Add(Property);
		}
	}

	return StructMembers;
}

const uint8* SUnrealDiffDataTableDetailTree::GetRowData(const FProperty* InProperty)
{
	UDataTable* DataTable = Cast<UDataTable>(DetailView->GetDataTable());
	if (!DataTable)
	{
		return nullptr;
	}

	auto RowMap = DataTable->GetRowMap();
	for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
	{
		for (TFieldIterator<FProperty> It(DataTable->RowStruct); It; ++It)
		{
			if (*It == InProperty)
			{
				return RowIt.Value();
			}
		}
	}
	
	return nullptr;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
