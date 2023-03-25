// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


#define LOCTEXT_NAMESPACE "SUnrealDiffDataTableListViewRow"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDataTableListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisual = InArgs._DataTableVisual;
	DataTableLayout = InArgs._DataTableLayout;
	RowDataPtr = InArgs._InRowDataPtr;
	
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

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::GenerateWidgetForColumn(const FName& InColumnId)
{
	if (InColumnId.IsEqual(TEXT("RowNumber")))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			[
				SNew(STextBlock)
#if ENGINE_MAJOR_VERSION == 4
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
				.Text(FText::FromString(FString::FromInt(RowDataPtr->RowNum)))
				.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
			];
	}
	
	if (InColumnId.IsEqual(TEXT("RowName")))
	{
		return SNew(SBox)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(SInlineEditableTextBlock)
				.IsReadOnly(true)
				.Text(FText::FromName(RowDataPtr->RowId))
				.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
			];
	}
	
	return MakeCellWidget(InColumnId);
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::MakeCellWidget(const FName& InColumnId)
{
	return SNew(SBox)
		.Padding(FMargin(4, 2, 4, 2))
		[
			SNew(STextBlock)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Margin(FMargin(5.f, 0.f, 0.f, 0.f))
#if ENGINE_MAJOR_VERSION == 4
			.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
			.Text(DataTableLayout->GetCellText(InColumnId, RowDataPtr->RowId))
			.ColorAndOpacity(DataTableLayout->GetCellTextColor(InColumnId, RowDataPtr->RowId))
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
