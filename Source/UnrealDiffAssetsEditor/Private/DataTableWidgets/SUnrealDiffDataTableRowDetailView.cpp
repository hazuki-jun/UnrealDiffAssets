// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "DataTableWidgets/SUnrealDiffDataTableDetailRowSelector.h"
#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableRowDetailView::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisualDiff = InArgs._DataTableVisualDiff;
	check(DataTableVisualDiff)

	SAssignNew(MyDetailTree, SUnrealDiffDataTableDetailTree, this);
	// DetailTree->DetailView = AsWeak();
	this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			BuildRowTitle()
		]

		+ SVerticalBox::Slot()
		[
			MyDetailTree.ToSharedRef()
		]
	];
}

TSharedRef<SWidget> SUnrealDiffDataTableRowDetailView::BuildRowTitle()
{
	TArray<FName> Rows;
	if (DataTableVisualDiff)
	{
		if (auto DataTable = Cast<UDataTable>(DataTableVisualDiff->GetAssetObject(bIsLocal)))
		{
			Rows = DataTable->GetRowNames();
		}
	}
	
	return SAssignNew(RowSelector, SUnrealDiffDataTableDetailRowSelector).Rows(Rows).SelectedRowName(RowName);
}

void SUnrealDiffDataTableRowDetailView::Refresh(const FName& InRowName)
{
	AllNodes.Empty();
	
	if (!MyDetailTree.IsValid())
	{
		return;
	}

	if (DataTableVisualDiff)
	{
		RowName = InRowName;
		if (RowSelector.IsValid())
		{
			RowSelector->OnRowSelected(RowName);
		}
		
		TSharedPtr<FUnrealDiffStructOnScope> StructOnScope = MakeShareable(new FUnrealDiffStructOnScope());
		StructOnScope->DataTable = Cast<UDataTable>(DataTableVisualDiff->GetLocalAsset());
		StructOnScope->CurrentRowName = InRowName;
		StructOnScope->StructureData = DataTableVisualDiff->GetStructure();
		
		MyDetailTree->SetStructure(StructOnScope);
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
}

UDataTable* SUnrealDiffDataTableRowDetailView::GetDataTable()
{
	if (DataTableVisualDiff)
	{
		return Cast<UDataTable>(DataTableVisualDiff->GetAssetObject(bIsLocal));
	}
	
	return SUnrealDiffDetailView::GetDataTable();
}

FName SUnrealDiffDataTableRowDetailView::GetCurrentRowName()
{
	return RowName;
}

SDataTableVisualDiff* SUnrealDiffDataTableRowDetailView::GetDataTableVisualDiff()
{
	return DataTableVisualDiff.Get();
}

void SUnrealDiffDataTableRowDetailView::SetItemExpansion(bool bIsExpand, int32 NodeIndex)
{
	if (!MyDetailTree)
	{
		return;
	}

	for (int32 i = 0; i < AllNodes.Num(); ++i)
	{
		if (AllNodes[i]->GetNodeIndex() == NodeIndex)
		{
			MyDetailTree->SetItemExpansion(bIsExpand, AllNodes[i]);
			return;
		}
	}
}

void SUnrealDiffDataTableRowDetailView::SetVerticalScrollOffset(float ScrollOffset)
{
	if (MyDetailTree)
	{
		MyDetailTree->SetVerticalScrollOffset(ScrollOffset);
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
