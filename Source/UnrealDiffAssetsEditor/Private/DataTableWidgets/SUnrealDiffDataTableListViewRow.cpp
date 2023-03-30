// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"

#include "IDocumentation.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
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

	check(RowDataPtr)
	
	SMultiColumnTableRow<FUnrealDiffDataTableRowListViewDataPtr>::Construct(
		FSuperRowType::FArguments()
#if ENGINE_MAJOR_VERSION == 4
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
#else
			.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
#endif
		InOwnerTableView
	);

	FText Tooltip;
	
	if (RowDataPtr->bIsRemoved)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Removed row {0}"), { *RowDataPtr->RowId.ToString() }));
	}
	else if (RowDataPtr->bIsAdded)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Added row {0}"), { *RowDataPtr->RowId.ToString() }));
	}
	else if (RowDataPtr->bHasAnyDifference)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Row {0} changed"), { *RowDataPtr->RowId.ToString() }));
	}
	else
	{
		// SetEnabled(false);
		SetColorAndOpacity(FLinearColor(0.2, 0.2, 0.2, 1.0));
	}
	
	SetToolTipText(Tooltip);
	
	// IDocumentation::Get()->CreateToolTip(Tooltip, nullptr, ExcerptLink, ExcerptName)
	
	SetBorderImage(TAttribute<const FSlateBrush*>(this, &SUnrealDiffDataTableListViewRow::GetBorder));
}

const FSlateBrush* SUnrealDiffDataTableListViewRow::GetBorder() const
{
	TSharedRef< ITypedTableView<FUnrealDiffDataTableRowListViewDataPtr> > OwnerTable = OwnerTablePtr.Pin().ToSharedRef();

	const bool bIsActive = OwnerTable->AsWidget()->HasKeyboardFocus();

	const bool bItemHasChildren = OwnerTable->Private_DoesItemHaveChildren( IndexInList );

	if (const FUnrealDiffDataTableRowListViewDataPtr* MyItemPtr = OwnerTable->Private_ItemFromWidget(this))
	{
		const bool bIsSelected = OwnerTable->Private_IsItemSelected(*MyItemPtr);
		const bool bIsHighlighted = OwnerTable->Private_IsItemHighlighted(*MyItemPtr);

		const bool bAllowSelection = GetSelectionMode() != ESelectionMode::None;
		const bool bEvenEntryIndex = (IndexInList % 2 == 0);

		if (bIsSelected && bShowSelection)
		{
			if (bIsActive)
			{
				return IsHovered()
					? &Style->ActiveHoveredBrush
					: &Style->ActiveBrush;
			}
			else
			{
				return IsHovered()
					? &Style->InactiveHoveredBrush
					: &Style->InactiveBrush;
			}
		}
		else if (!bIsSelected && bIsHighlighted)
		{
			if (bIsActive)
			{
				return IsHovered()
					? (bEvenEntryIndex ? &Style->EvenRowBackgroundHoveredBrush : &Style->OddRowBackgroundHoveredBrush)
					: &Style->ActiveHighlightedBrush;
			}
			else
			{
				return IsHovered()
					? (bEvenEntryIndex ? &Style->EvenRowBackgroundHoveredBrush : &Style->OddRowBackgroundHoveredBrush)
					: &Style->InactiveHighlightedBrush;
			}
		}
#if ENGINE_MAJOR_VERSION == 5
		else if (bItemHasChildren && Style->bUseParentRowBrush && GetIndentLevel() == 0)
		{
			return IsHovered() 
			? &Style->ParentRowBackgroundHoveredBrush	
			: &Style->ParentRowBackgroundBrush;	
		}
#endif
		else
		{
			// Add a slightly lighter background for even rows
			if (bEvenEntryIndex)
			{
				return (IsHovered() && bAllowSelection)
					? &Style->EvenRowBackgroundHoveredBrush
					: &Style->EvenRowBackgroundBrush;

			}
			else
			{
				return (IsHovered() && bAllowSelection)
					? &Style->OddRowBackgroundHoveredBrush
					: &Style->OddRowBackgroundBrush;

			}
		}
	}

	return &Style->InactiveHoveredBrush;
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId);	
	}
	
	return FReply::Handled();
}

FReply SUnrealDiffDataTableListViewRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);

	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId);	

		TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();

		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget> SUnrealDiffDataTableListViewRow::MakeRowActionsMenu()
{
	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_CopyName", "Copy Name"),
		LOCTEXT("DataTableRowMenuActions_CopyNamTooltip", "Copy row name"),
		FUnrealDiffWindowStyle::GetAppSlateIcon("GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionCopyName))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_Copy", "Copy Value"),
		LOCTEXT("DataTableRowMenuActions_CopyTooltip", "Copy this row"),
		FUnrealDiffWindowStyle::GetAppSlateIcon("GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionCopyValue))
	);
	
	if (!bIsLocal)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DataTableRowMenuActions_MergeRow", "Merge Row"),
			LOCTEXT("DataTableRowMenuActions_MergeRowTooltip", "Merge this row to left"),
			FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser"),
			FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionMerge))
		);
	}
	
	// MenuBuilder.AddMenuEntry(
	// LOCTEXT("DataTableRowMenuActions_ShowDifference", "Show Difference"),
	// LOCTEXT("DataTableRowMenuActions_ShowDifferencTooltip", "Show Difference"),
	// 	FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.Actions.Diff"),
	// 	FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDataTableListViewRow::OnMenuActionShowDifference))
	// );
	
	return MenuBuilder.MakeWidget();
}

void SUnrealDiffDataTableListViewRow::OnMenuActionCopyName()
{
	if (DataTableVisual)
	{
		DataTableVisual->CopyRowName(RowDataPtr->RowId);
	}
}

void SUnrealDiffDataTableListViewRow::OnMenuActionCopyValue()
{
	if (!RowDataPtr.IsValid())
	{
		return;
	}

	if (DataTableVisual)
	{
		if (!RowDataPtr->bIsRemoved)
		{
			DataTableVisual->CopyRow(bIsLocal, RowDataPtr->RowId);
		}
		else
		{
			DataTableVisual->CopyRow(bIsLocal, NAME_None);
		}
	}
}

void SUnrealDiffDataTableListViewRow::OnMenuActionShowDifference()
{
	if (DataTableVisual)
	{
		DataTableVisual->ShowDifference_RowToRow(RowDataPtr->RowId);
	}
}

void SUnrealDiffDataTableListViewRow::OnMenuActionMerge()
{
	if (DataTableVisual)
	{
		if (RowDataPtr->bIsRemoved)
		{
			DataTableVisual->MergeAction_DeleteRow(RowDataPtr->RowId);	
		}
		else
		{
			DataTableVisual->MergeAction_MergeRow(RowDataPtr->RowId);
		}
	}
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
