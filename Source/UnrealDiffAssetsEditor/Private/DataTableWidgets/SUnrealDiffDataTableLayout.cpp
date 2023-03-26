// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"

#include "IDocumentation.h"
#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Fonts/FontMeasure.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableLayout"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableLayout::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisual = InArgs._DataTableVisual;
	InitialDesiredWidth = DataTableVisual->GetParentWindow()->GetClientSizeInScreen().X / 2;
	
	UUnrealDiffAssetDelegate::OnWindowResized.AddRaw(this, &SUnrealDiffDataTableLayout::OnWindowResized);
	
	FText Title = InArgs._Title;

	if (bIsLocal)
	{
		DataTableVisual->DataTableLayoutLocal = SharedThis(this);
	}
	else
	{
		DataTableVisual->DataTableLayoutRemote = SharedThis(this);
	}
	
	DataTableVisual->GetDataTableData(bIsLocal, AvailableColumns, AvailableRows);
	
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

	VisibleRows.StableSort([](const FUnrealDiffDataTableRowListViewDataPtr& A, const FUnrealDiffDataTableRowListViewDataPtr& B)
	{
		return (A->RowId).ToString() < (B->RowId).ToString();
	});

	for (int32 i = 0; i < VisibleRows.Num(); ++i)
	{
		if (VisibleRows[i]->RowNum == -2)
		{
			VisibleRows[i]->bIsRemoved = true;
		}
		else if (VisibleRows[i]->RowNum == 0)
		{
			VisibleRows[i]->bIsAdded = true;
		}
		else if (VisibleRows[i]->RowNum == -1)
		{
			VisibleRows[i]->bHasAnyDifference = true;
		}
		
		VisibleRows[i]->RowNum = i + 1;
	}
	
	SetupColumnWidth();
	RefreshRowNumberColumnWidth();
	RefreshRowNameColumnWidth();

	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)))
			.Visibility(EVisibility::SelfHitTestInvisible)
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			BuildContent()
		]
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
		.OnListViewScrolled(this, &SUnrealDiffDataTableLayout::OnListViewScrolled)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::Yes);
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal)
				.ExternalScrollbar(HorizontalScrollBar)
				+ SScrollBox::Slot()
				[
					ListView.ToSharedRef()
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				VerticalScrollBar
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				HorizontalScrollBar
			]
		];
}

void SUnrealDiffDataTableLayout::OnListViewScrolled(double InScrollOffset)
{
	if (DataTableVisual.IsValid())
	{
		DataTableVisual->SyncVerticalScrollOffset(bIsLocal, InScrollOffset);
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

		TAttribute<float> RowWidth = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &SUnrealDiffDataTableLayout::GetColumnWidth, ColumnIndex));
		
		ColumnNamesHeaderRow->AddColumn(
			SHeaderRow::Column(ColumnData->ColumnId)
			.DefaultLabel(ColumnData->DisplayName)
			.ManualWidth(RowWidth)
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
	float TempSize = 0.f;
	ColumnWidths.SetNum(AvailableColumns.Num());
	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.CurrentWidth = FMath::Clamp(ColumnData->DesiredColumnWidth, 10.0f, 400.0f) + 10.f; // Clamp auto-sized columns to a reasonable limit

		TempSize += ColumnWidth.CurrentWidth;
		if (ColumnIndex == AvailableColumns.Num() - 1 && TempSize < InitialDesiredWidth)
		{
			ColumnWidth.CurrentWidth = InitialDesiredWidth - TempSize - 3;
		}
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
	return SNew(SUnrealDiffDataTableListViewRow, OwnerTable).IsLocal(bIsLocal).DataTableVisual(DataTableVisual).InRowDataPtr(InRowDataPtr).DataTableLayout(SharedThis(this));
}

void SUnrealDiffDataTableLayout::SetListViewScrollOffset(float InOffset)
{
	if (ListView.IsValid())
	{
		ListView->SetScrollOffset(InOffset);
	}
}

TSharedRef<SWidget> SUnrealDiffDataTableLayout::CreateRowDetailView()
{
	return SNullWidget::NullWidget;
}

void SUnrealDiffDataTableLayout::OnWindowResized(FVector2D WindowSize)
{
	InitialDesiredWidth = WindowSize.X / 2;
	SetupColumnWidth();
}

void SUnrealDiffDataTableLayout::SelectRow(FName RowId)
{
	if (!ListView.IsValid())
	{
		return;
	}
	
	for (const auto& RowData : VisibleRows)
	{
		ListView->SetItemSelection(RowData, false);
	}
	
	for (const auto& RowData : VisibleRows)
	{
		if (RowData->RowId != RowId)
		{
			continue;
		}

		if (!ListView->IsItemSelected(RowData))
		{
			ListView->SetItemSelection(RowData, true);
			break;
		}
	}
}

FText SUnrealDiffDataTableLayout::GetCellText(const FName& InColumnId, const FName& RowName) const
{
	int32 ColumnIndex = -1;
	for (int32 i = 0; i < AvailableColumns.Num(); ++i)
	{
		if (AvailableColumns[i]->ColumnId == InColumnId)
		{
			ColumnIndex = i;
			break;
		}
	}
	
	if (ColumnIndex >= 0)
	{
		for (const auto& RowData : AvailableRows)
		{
			if (RowData->RowId == RowName)
			{
				if (RowData->CellData.IsValidIndex(ColumnIndex))
				{
					return RowData->CellData[ColumnIndex];
				}
			}
		}
	}
	
	return FText();
}

FSlateColor SUnrealDiffDataTableLayout::GetCellTextColor(const FName& InColumnId, const FName& RowName) const
{
	for (const auto& RowData : VisibleRows)
	{
		if (RowName == RowData->RowId)
		{
			if (RowData->bIsRemoved)
			{
				return FSlateColor(FLinearColor(0.8, 0.0, 0.1, 1.0));
				
			}
			else if (RowData->bIsAdded)
			{
				return FSlateColor(FLinearColor(0.0, 0.8, 0.1, 1.0));
			}
			else if (RowData->bHasAnyDifference)
			{
				return FSlateColor(FLinearColor(1.0, 1.0, 0.1, 1.0));
			}
		}
	}
	
	return FSlateColor(FLinearColor(0.5, 0.5, 0.5, 1.0));
}

bool SUnrealDiffDataTableLayout::IsCellEnable(const FName& InColumnId, const FName& RowName) const
{
	return true;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
