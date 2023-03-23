// Fill out your copyright notice in the Description page of Project Settings.


#include "SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "SlateOptMacros.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDataTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	
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
			BuildLayoutWidget(true, CellsListView_Local)
		]

		+ SSplitter::Slot()
		[
			BuildLayoutWidget(false, CellsListView_Remote)
		]
	];
}

void SDataTableVisualDiff::RefreshRowNumberColumnWidth()
{
	TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const FTextBlockStyle& CellTextStyle = FAppStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
	const float CellPadding = 10.0f;

	for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
	{
		const float RowNumberWidth = (float)FontMeasure->Measure(FString::FromInt(RowData->RowNum), CellTextStyle.Font).X + CellPadding;
		RowNumberColumnWidth = FMath::Max(RowNumberColumnWidth, RowNumberWidth);
	}
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildLayoutWidget(bool bIsLocal, TSharedPtr<SListView<FUnrealDiffDataTableRowListViewDataPtr>> InListView)
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

		if (bIsLocal)
		{
			VisibleRows_Local.Add(Row);
		}
		else
		{
			VisibleRows_Remote.Add(Row);
		}
	}
	
	FText LayoutTitle = bIsLocal ? FText::FromString(TEXT("Local DataTable")) : FText::FromString(TEXT("Remote DataTable"));
	
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

	RefreshRowNumberColumnWidth();
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = GenerateHeaderWidgets();
	InListView = SNew(SListView<FUnrealDiffDataTableRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows_Local)
		.HeaderRow(ColumnNamesHeaderRow)
		.OnGenerateRow(this, &SDataTableVisualDiff::MakeRowWidget)
		// .OnSelectionChanged(this, &FDataTableEditor::OnRowSelectionChanged)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	TSharedPtr<SSearchBox> SearchBoxWidget;
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
			. HAlign(HAlign_Fill)
		   [
			   SNew(SHorizontalBox)
			   + SHorizontalBox::Slot()
			   . HAlign(HAlign_Fill)
			   [
				   SNew(SScrollBox)
				   .Orientation(Orient_Horizontal)
				   .ExternalScrollbar(HorizontalScrollBar)
				   +SScrollBox::Slot()
				   [
					   InListView.ToSharedRef()
				   ]
			   ]
			   // + SHorizontalBox::Slot()
			   // .AutoWidth()
			   // [
				  //  VerticalScrollBar
			   // ]
		   ];
		   // +SVerticalBox::Slot()
		   // .AutoHeight()
		   // [
			  //  SNew(SHorizontalBox)
			  //  +SHorizontalBox::Slot()
			  //  [
				 //   HorizontalScrollBar
			  //  ]
		   // ];
}

TSharedPtr<SHeaderRow> SDataTableVisualDiff::GenerateHeaderWidgets()
{
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = SNew(SHeaderRow);
	
	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowNumber"))
		.DefaultLabel(LOCTEXT("DataTableRowNumber", "Row Number"))
		.ManualWidth(this, &SDataTableVisualDiff::GetRowNumberColumnWidth)
		.OnWidthChanged(this, &SDataTableVisualDiff::OnRowNumberColumnResized)
	);

	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowName"))
		.DefaultLabel(LOCTEXT("DataTableRowName", "Row Name"))
		.ManualWidth(this, &SDataTableVisualDiff::GetRowNumberColumnWidth)
		.OnWidthChanged(this, &SDataTableVisualDiff::OnRowNumberColumnResized)
	);
	
	
	return ColumnNamesHeaderRow;
}

float SDataTableVisualDiff::GetRowNumberColumnWidth() const
{
	return RowNumberColumnWidth;
}

void SDataTableVisualDiff::OnRowNumberColumnResized(const float NewWidth)
{
	RowNumberColumnWidth = NewWidth;
}

TSharedRef<ITableRow> SDataTableVisualDiff::MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDataTableListViewRow, OwnerTable)
		.RowDataPtr(InRowDataPtr)
		.IsEditable(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
