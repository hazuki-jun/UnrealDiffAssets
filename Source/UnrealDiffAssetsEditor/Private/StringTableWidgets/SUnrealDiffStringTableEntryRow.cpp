// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SUnrealDiffStringTableEntryRow.h"
#include "StringTableWidgets/SStringTableVisualDiff.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
#include "StringTableWidgets/SUnrealDiffStringTableListView.h"

namespace NS_UnrealDiffStringTableEntryRow
{
	const FName StringTableDummyColumnId("Dummy");
	const FName StringTableKeyColumnId("Key");
	const FName StringTableSourceStringColumnId("SourceString");
}

#define LOCTEXT_NAMESPACE "SUnrealDiffStringTableEntryRow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffStringTableEntryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<FCachedStringTableEntry> InCachedStringTableEntry, TSharedPtr<class SUnrealDiffStringTableListView> InStringTableListView)
{

	bIsLocal = InArgs._IsLocal;
	CachedStringTableEntry = InCachedStringTableEntry;
	StringTableListView = InStringTableListView;
	
	FSuperRowType::Construct(FSuperRowType::
		FArguments()
		.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DataTableEditor.CellListViewRow"),
		OwnerTableView);
}

TSharedRef<SWidget> SUnrealDiffStringTableEntryRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	TSharedPtr<SWidget> Return;

	FSlateColor TextColor = FLinearColor(0.9f, 0.9f, 0.9f);
	if (CachedStringTableEntry->RowState == EUnrealVisualDiff::Modify)
	{
		TextColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else if (CachedStringTableEntry->RowState == EUnrealVisualDiff::Added)
	{
		TextColor = FLinearColor(0.0, 0.8, 0.1, 1.0);
	}
	else if (CachedStringTableEntry->RowState == EUnrealVisualDiff::Removed)
	{
		TextColor = FLinearColor(0.8, 0.0, 0.1, 1.0);
	}
	
	if (ColumnName.IsEqual(NS_UnrealDiffStringTableEntryRow::StringTableKeyColumnId))
	{
		Return = SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(CachedStringTableEntry.Get()->Key))
				.ColorAndOpacity(TextColor)
			];
	}
	else if (ColumnName.IsEqual(NS_UnrealDiffStringTableEntryRow::StringTableSourceStringColumnId))
	{
		Return = SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString(CachedStringTableEntry.Get()->SourceString))
				.ColorAndOpacity(TextColor)
			];
	}
	
	return Return.IsValid() ? Return.ToSharedRef() : SNullWidget::NullWidget;
}

FReply SUnrealDiffStringTableEntryRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UUnrealDiffAssetDelegate::OnStringTableRowSelected.Execute(bIsLocal, CachedStringTableEntry->Key, CachedStringTableEntry->RowNumber);
	}
	
	return FReply::Handled();
}

FReply SUnrealDiffStringTableEntryRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UUnrealDiffAssetDelegate::OnStringTableRowSelected.Execute(bIsLocal, CachedStringTableEntry->Key, CachedStringTableEntry->RowNumber);	

		TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();

		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
	}
	
	return STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
}

TSharedRef<SWidget> SUnrealDiffStringTableEntryRow::MakeRowActionsMenu()
{
	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_CopyName", "Copy Name"),
		LOCTEXT("DataTableRowMenuActions_CopyNamTooltip", "Copy row name"),
		FUnrealDiffWindowStyle::GetAppSlateIcon("GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffStringTableEntryRow::OnMenuActionCopyName))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_Copy", "Copy Value"),
		LOCTEXT("DataTableRowMenuActions_CopyTooltip", "Copy this row"),
                       		FUnrealDiffWindowStyle::GetAppSlateIcon("GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffStringTableEntryRow::OnMenuActionCopyValue))
	);

	if (!bIsLocal)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DataTableRowMenuActions_MergeRow", "Use This Change"),
			LOCTEXT("DataTableRowMenuActions_MergeRowTooltip", "Use This Change"),
			FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser"),
			FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffStringTableEntryRow::OnMenuActionMerge))
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void SUnrealDiffStringTableEntryRow::OnMenuActionCopyName()
{
	if (CachedStringTableEntry->RowState == EUnrealVisualDiff::Removed)
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*CachedStringTableEntry->Key);
}

void SUnrealDiffStringTableEntryRow::OnMenuActionCopyValue()
{
	if (CachedStringTableEntry->RowState == EUnrealVisualDiff::Removed)
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*CachedStringTableEntry->SourceString);
}

void SUnrealDiffStringTableEntryRow::OnMenuActionMerge()
{
	if (StringTableListView.IsValid())
	{
		StringTableListView->MergeRow(CachedStringTableEntry->Key);
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

