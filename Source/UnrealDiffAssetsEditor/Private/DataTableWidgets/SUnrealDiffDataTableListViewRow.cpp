// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableListViewRow"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	RowDataPtr = InArgs._RowDataPtr;
	CurrentName = MakeShareable(new FName(RowDataPtr->RowId));
	IsEditable = false;
	AvailableColumns = InArgs._AvailableColumns;
	bIsLocal = InArgs._IsLocal;
	
	SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>::Construct(
		FSuperRowType::FArguments()
#if ENGINE_MAJOR_VERSION == 4
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
#else
			.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
#endif
		InOwnerTableView
	);

	SetBorderImage(TAttribute<const FSlateBrush*>(this, &SUnrealDiffDataTableListViewRow::GetBorder));
}

const FSlateBrush* SUnrealDiffDataTableListViewRow::GetBorder() const
{
	return STableRow::GetBorder();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId);
	
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	STableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	STableRow::OnKeyDown(MyGeometry, InKeyEvent);
	return FReply::Handled();
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
#if ENGINE_MAJOR_VERSION == 4
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
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
				.IsReadOnly(true)
				// .OnTextCommitted(this, &SDataTableListViewRow::OnRowRenamed)
				// .HighlightText(DataTableEdit, &FDataTableEditor::GetFilterText)
				// .ColorAndOpacity(DataTableEdit, &FDataTableEditor::GetRowTextColor, RowDataPtr->RowId)
			];
	}

	int32 ColumnIndex = 0;
	for (; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		if (ColumnData->ColumnId == InColumnId)
		{
			break;
		}
	}
	
	if (AvailableColumns.IsValidIndex(ColumnIndex) && RowDataPtr->CellData.IsValidIndex(ColumnIndex))
	{
		TAttribute<FText> TextAttribute = GetCellText(ColumnIndex);
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.Margin(FMargin(5.f, 0.f, 0.f, 0.f))
#if ENGINE_MAJOR_VERSION == 4
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
				.Text(TextAttribute)
				// .ColorAndOpacity(DataTableEdit, &FDataTableEditor::GetRowTextColor, RowDataPtr->RowId)
			];
	}
	
	return SNullWidget::NullWidget;
}

FText SUnrealDiffDataTableListViewRow::GetCellText(int32 ColumnIndex) const
{
	if (RowDataPtr.IsValid() && ColumnIndex < RowDataPtr->CellData.Num())
	{
		return RowDataPtr->CellData[ColumnIndex];
	}

	return FText();
}

void SUnrealDiffDataTableListViewRow::CopySelectedRow()
{
	// UDataTable* TablePtr = Cast<UDataTable>(Data);
	// uint8* RowPtr = TablePtr ? TablePtr->GetRowMap().FindRef(HighlightedRowName) : nullptr;
	//
	// if (!RowPtr || !TablePtr->RowStruct)
	// 	return;
	//
	// FString ClipboardValue;
	// TablePtr->RowStruct->ExportText(ClipboardValue, RowPtr, RowPtr, TablePtr, PPF_Copy, nullptr);
	//
	// FPlatformApplicationMisc::ClipboardCopy(*ClipboardValue);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
