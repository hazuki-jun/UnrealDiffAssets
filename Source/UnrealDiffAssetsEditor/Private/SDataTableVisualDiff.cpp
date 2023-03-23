// Fill out your copyright notice in the Description page of Project Settings.


#include "SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "SlateOptMacros.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDataTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;

	ColumnNamesHeaderRow = SNew(SHeaderRow);
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		BuildWidgetContent()
	];
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildWidgetContent()
{
	return SNew(SOverlay)
	+ SOverlay::Slot()
	[
		SNew(SSplitter)
		.Style(FAppStyle::Get(), "DetailsView.Splitter")
		.PhysicalSplitterHandleSize(5.0f)
		.HitDetectionSplitterHandleSize(5.0f)
		+ SSplitter::Slot()
		[
			BuildLayoutWidget(true)
		]

		+ SSplitter::Slot()
		[
			BuildLayoutWidget(false)
		]
	];
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildLayoutWidget(bool bIsLocal)
{
	UDataTable* DataTable = CastChecked<UDataTable>(LocalAsset);
	
	FDataTableEditorUtils::CacheDataTableForEditing(DataTable, AvailableColumns, AvailableRows);
	
	for (auto RowData : AvailableRows)
	{
		FUnrealDiffDataTableRowListViewDataPtr Row = MakeShared<FUnrealDiffDataTableRowListViewData>();
		Row->CellData = RowData->CellData;
		Row->DisplayName = RowData->DisplayName;
		Row->RowId = RowData->RowId;
		Row->RowNum = RowData->RowNum;
		Row->DesiredRowHeight = RowData->DesiredRowHeight;

		VisibleRows_Local.Add(Row);
	}
	
	FText LayoutTitle = bIsLocal ? FText::FromString(TEXT("Local DataTable")) : FText::FromString(TEXT("Remote DataTable"));
	
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));
	
	CellsListView = SNew(SListView<FUnrealDiffDataTableRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows_Local)
		.HeaderRow(ColumnNamesHeaderRow)
		.OnGenerateRow(this, &SDataTableVisualDiff::MakeRowWidget)
		// .OnSelectionChanged(this, &FDataTableEditor::OnRowSelectionChanged)
		// .ExternalScrollbar(VerticalScrollBar)
		// .ConsumeMouseWheel(EConsumeMouseWheel::Always)
		// .SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);
	
	return SNew(SVerticalBox)
		   + SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   [
				   SAssignNew(SearchBoxWidget, SSearchBox)
				   // .InitialText(this, &FDataTableEditor::GetFilterText)
				   // .OnTextChanged(this, &FDataTableEditor::OnFilterTextChanged)
				   // .OnTextCommitted(this, &FDataTableEditor::OnFilterTextCommitted)
			   ]
		   ]
		   +SVerticalBox::Slot()
		   [
			   SNew(SHorizontalBox)
			   +SHorizontalBox::Slot()
			   [
				   SNew(SScrollBox)
				   .Orientation(Orient_Horizontal)
				   .ExternalScrollbar(HorizontalScrollBar)
				   +SScrollBox::Slot()
				   [
					   CellsListView.ToSharedRef()
				   ]
			   ]
			   +SHorizontalBox::Slot()
			   .AutoWidth()
			   [
				   VerticalScrollBar
			   ]
		   ]
		   +SVerticalBox::Slot()
		   .AutoHeight()
		   [
			   SNew(SHorizontalBox)
			   +SHorizontalBox::Slot()
			   [
				   HorizontalScrollBar
			   ]
		   ];
}

TSharedRef<SWidget> SDataTableVisualDiff::GenerateHeaderWidgets()
{
	return SNew(SOverlay);
}

TSharedRef<ITableRow> SDataTableVisualDiff::MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDataTableListViewRow, OwnerTable)
		.RowDataPtr(InRowDataPtr)
		.IsEditable(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
