// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "DataTableWidgets/SUnrealDiffDataTableDetailRowSelector.h"
#include "DataTableWidgets/SUnrealDiffDataTableDetailTree.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableRowDetailView::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisualDiff = InArgs._DataTableVisualDiff;
	check(DataTableVisualDiff)
	
	if (bIsLocal)
	{
		DataTableVisualDiff->RowDetailViewLocal = SharedThis(this);
	}
	else
	{
		DataTableVisualDiff->RowDetailViewRemote = SharedThis(this);
	}

	SAssignNew(DetailTree, SUnrealDiffDataTableDetailTree);

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
			DetailTree.ToSharedRef()
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
	if (!DetailTree.IsValid())
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
		
		DetailTree->SetStructure(DataTableVisualDiff->GetStructure());
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
