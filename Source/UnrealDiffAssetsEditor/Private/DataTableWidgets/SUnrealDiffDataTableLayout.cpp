// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"

#include "IDocumentation.h"
#include "SlateOptMacros.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableLayout"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableLayout::Construct(const FArguments& InArgs)
{
	AssetObject = InArgs._AssetObject;
	bIsLocal = InArgs._IsLocal;
	FText Title = InArgs._Title;
	
	UDataTable* DataTable = CastChecked<UDataTable>(AssetObject);
	FDataTableEditorUtils::CacheDataTableForEditing(DataTable, AvailableColumns, AvailableRows);
	
	for (auto RowData : AvailableRows)
	{
		FUnrealDiffDataTableRowListViewDataPtr Row = MakeShared<FUnrealDiffDataTableRowListViewData>();
		Row->CellData = RowData->CellData;
		Row->DisplayName = RowData->DisplayName;
		Row->RowId = RowData->RowId;
		Row->RowNum = RowData->RowNum;
		Row->DesiredRowHeight = RowData->DesiredRowHeight;
		VisibleRows.Add(Row);
	}

	VisibleRows.StableSort([](const FUnrealDiffDataTableRowListViewDataPtr& first, const FUnrealDiffDataTableRowListViewDataPtr& second)
	{
		return (first->DisplayName).ToString() < (second->DisplayName).ToString();
	});
	
	SetupColumnWidth();
	RefreshRowNumberColumnWidth();
	RefreshRowNameColumnWidth();

	this->ChildSlot
	[
		BuildContent()
	];
}

TSharedRef<SWidget> SUnrealDiffDataTableLayout::BuildContent()
{
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
	.Orientation(Orient_Horizontal)
	.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

		
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = GenerateHeaderWidgets();
	ListView = SNew(SListView<FUnrealDiffDataTableRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows)
		.HeaderRow(ColumnNamesHeaderRow)
		.OnGenerateRow(this, &SUnrealDiffDataTableLayout::MakeRowWidget)
		// .OnSelectionChanged(this, &SUnrealDiffDataTableLayout::OnRowSelectionChanged)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	// TSharedPtr<SSearchBox> SearchBoxWidget;
	
	return SNew(SVerticalBox)
	   // + SVerticalBox::Slot()
	   // .AutoHeight()
	   // [
		  //  SNew(SHorizontalBox)
		  //  + SHorizontalBox::Slot()
		  //  [
			 //   SAssignNew(SearchBoxWidget, SSearchBox)
		  //  ]
	   // ]
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
				   ListView.ToSharedRef()
			   ]
		   ]
	   ];
}

void SUnrealDiffDataTableLayout::OnRowSelectionChanged(FUnrealDiffDataTableRowListViewDataPtr InNewSelection, ESelectInfo::Type InSelectInfo)
{
	if (InSelectInfo == ESelectInfo::OnMouseClick)
	{
		if (InNewSelection.IsValid())
		{
			
		}
	}
}

TSharedPtr<SHeaderRow> SUnrealDiffDataTableLayout::GenerateHeaderWidgets()
{
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = SNew(SHeaderRow);
	
	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowNumber"))
		.FillWidth(1.0f)
		.DefaultLabel(LOCTEXT("DataTableRowNumber", " "))
		.ManualWidth(this, &SUnrealDiffDataTableLayout::GetRowNumberColumnWidth)
		.OnWidthChanged(this, &SUnrealDiffDataTableLayout::OnRowNumberColumnResized)
	);

	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowName"))
		.DefaultLabel(LOCTEXT("DataTableRowName", "Row Name"))
		.ManualWidth(this, &SUnrealDiffDataTableLayout::GetRowNameColumnWidth)
		.OnWidthChanged(this, &SUnrealDiffDataTableLayout::OnRowNameColumnResized)
	);

	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];

		ColumnNamesHeaderRow->AddColumn(
			SHeaderRow::Column(ColumnData->ColumnId)
			.DefaultLabel(ColumnData->DisplayName)
			.ManualWidth(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &SUnrealDiffDataTableLayout::GetColumnWidth, ColumnIndex)))
			.OnWidthChanged(this, &SUnrealDiffDataTableLayout::OnColumnResized, ColumnIndex)
			[
				SNew(SBox)
				.Padding(FMargin(0, 4, 0, 4))
				.VAlign(VAlign_Fill)
				.ToolTip(IDocumentation::Get()->CreateToolTip(FDataTableEditorUtils::GetRowTypeInfoTooltipText(ColumnData), nullptr, *FDataTableEditorUtils::VariableTypesTooltipDocLink, FDataTableEditorUtils::GetRowTypeTooltipDocExcerptName(ColumnData)))
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(ColumnData->DisplayName)
				]
			]
		);
	}
	
	return ColumnNamesHeaderRow;
}

void SUnrealDiffDataTableLayout::SetupColumnWidth()
{
	// Setup the default auto-sized columns
	ColumnWidths.SetNum(AvailableColumns.Num());
	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.CurrentWidth = FMath::Clamp(ColumnData->DesiredColumnWidth, 10.0f, 400.0f); // Clamp auto-sized columns to a reasonable limit
	}
}

float SUnrealDiffDataTableLayout::GetRowNameColumnWidth() const
{
	return RowNameColumnWidth;
}

void SUnrealDiffDataTableLayout::RefreshRowNameColumnWidth()
{
#if ENGINE_MAJOR_VERSION == 4
	const FTextBlockStyle& CellTextStyle = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#else
	const FTextBlockStyle& CellTextStyle = FAppStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#endif
	
	TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	static const float CellPadding = 10.0f;

	for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
	{
		const float RowNameWidth = (float)FontMeasure->Measure(RowData->DisplayName, CellTextStyle.Font).X + CellPadding;
		RowNameColumnWidth = FMath::Max(RowNameColumnWidth, RowNameWidth);
	}
}

void SUnrealDiffDataTableLayout::RefreshRowNumberColumnWidth()
{
#if ENGINE_MAJOR_VERSION == 4
	const FTextBlockStyle& CellTextStyle = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#else
	const FTextBlockStyle& CellTextStyle = FAppStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#endif
	
	TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const float CellPadding = 10.0f;

	for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
	{
		const float RowNumberWidth = (float)FontMeasure->Measure(FString::FromInt(RowData->RowNum), CellTextStyle.Font).X + CellPadding;
		RowNumberColumnWidth = FMath::Max(RowNumberColumnWidth, RowNumberWidth);
	}
}

float SUnrealDiffDataTableLayout::GetColumnWidth(const int32 ColumnIndex) const
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		return ColumnWidths[ColumnIndex].CurrentWidth;
	}
	return 0.0f;
}

void SUnrealDiffDataTableLayout::OnColumnResized(const float NewWidth, const int32 ColumnIndex)
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.bIsAutoSized = false;
		ColumnWidth.CurrentWidth = NewWidth;
	}
}

void SUnrealDiffDataTableLayout::OnRowNameColumnResized(const float NewWidth)
{
	RowNameColumnWidth = NewWidth;
}

float SUnrealDiffDataTableLayout::GetRowNumberColumnWidth() const
{
	return RowNumberColumnWidth;
}

void SUnrealDiffDataTableLayout::OnRowNumberColumnResized(const float NewWidth)
{
	RowNumberColumnWidth = NewWidth;
}

TSharedRef<ITableRow> SUnrealDiffDataTableLayout::MakeRowWidget(FUnrealDiffDataTableRowListViewDataPtr InRowDataPtr,const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDataTableListViewRow, OwnerTable).RowDataPtr(InRowDataPtr).AvailableColumns(AvailableColumns).IsLocal(bIsLocal);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
