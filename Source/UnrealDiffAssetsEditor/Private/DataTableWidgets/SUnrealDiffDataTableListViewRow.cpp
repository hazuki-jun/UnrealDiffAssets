// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

#include "SlateOptMacros.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	RowDataPtr = InArgs._RowDataPtr;
	CurrentName = MakeShareable(new FName(RowDataPtr->RowId));
	IsEditable = false;
	SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>::Construct(
		FSuperRowType::FArguments()
		.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
		InOwnerTableView
	);

	SetBorderImage(TAttribute<const FSlateBrush*>(this, &SUnrealDiffDataTableListViewRow::GetBorder));
}

const FSlateBrush* SUnrealDiffDataTableListViewRow::GetBorder() const
{
	if (bIsHoveredDragTarget)
	{
		return FAppStyle::GetBrush("DataTableEditor.DragDropHoveredTarget");
	}
	else
	{
		return STableRow::GetBorder();
	}
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

FReply SUnrealDiffDataTableListViewRow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return FReply::Unhandled();
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	return MakeCellWidget(0, ColumnName);
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::MakeCellWidget(const int32 InRowIndex, const FName& InColumnId)
{
	const FName RowNumberColumnId("RowNumber");

	if (InColumnId.IsEqual(RowNumberColumnId))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
				.Text(FText::FromString(FString::FromInt(RowDataPtr->RowNum)))
				// .ColorAndOpacity(DataTableEdit, &FDataTableEditor::GetRowTextColor, RowDataPtr->RowId)
				// .HighlightText(DataTableEdit, &FDataTableEditor::GetFilterText)
			];
	}

	const FName RowNameColumnId("RowName");

	if (InColumnId.IsEqual(RowNameColumnId))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SAssignNew(InlineEditableText, SInlineEditableTextBlock)
				.Text(RowDataPtr->DisplayName)
				// .OnTextCommitted(this, &SDataTableListViewRow::OnRowRenamed)
				// .HighlightText(DataTableEdit, &FDataTableEditor::GetFilterText)
				// .ColorAndOpacity(DataTableEdit, &FDataTableEditor::GetRowTextColor, RowDataPtr->RowId)
				// .IsReadOnly(!IsEditable)
			];
	}

	return SNullWidget::NullWidget;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
