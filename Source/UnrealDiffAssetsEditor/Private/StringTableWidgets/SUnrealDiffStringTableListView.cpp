// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SUnrealDiffStringTableListView.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "Internationalization/StringTableCore.h"
#include "StringTableWidgets/SStringTableVisualDiff.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffStringTableListView"

namespace
{
	const FName StringTableDummyColumnId("Dummy");
	const FName StringTableKeyColumnId("Key");
	const FName StringTableSourceStringColumnId("SourceString");
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffStringTableEntryRow::Construct(const FTableRowArgs& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<SUnrealDiffStringTableListView::FCachedStringTableEntry> InCachedStringTableEntry)
{
	CachedStringTableEntry = InCachedStringTableEntry;
		
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
	
	if (ColumnName.IsEqual(StringTableKeyColumnId))
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
	else if (ColumnName.IsEqual(StringTableSourceStringColumnId))
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

FReply SUnrealDiffStringTableEntryRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		// UUnrealDiffAssetDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId, RowDataPtr->RowNum);	

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

	return MenuBuilder.MakeWidget();
}

void SUnrealDiffStringTableEntryRow::OnMenuActionCopyName()
{
}

void SUnrealDiffStringTableEntryRow::OnMenuActionCopyValue()
{
}

void SUnrealDiffStringTableListView::Construct(const FArguments& InArgs, TSharedPtr<class SStringTableVisualDiff> InStringTableVisualDiff)
{
	StringTableVisualDiff = InStringTableVisualDiff;
	bIsLocal = InArgs._IsLocal;
	SetupRowsListSources();
	ChildSlot
	[
		SAssignNew(MyListView, SListView<TSharedPtr<FCachedStringTableEntry>>)
				.ListItemsSource(&CachedStringTableEntries)
				.OnGenerateRow(this, &SUnrealDiffStringTableListView::OnGenerateStringTableEntryRow)
				.HeaderRow(
					SNew(SHeaderRow)
						+SHeaderRow::Column(StringTableDummyColumnId)
						.DefaultLabel(FText::GetEmpty())
						.FixedWidth(20)

						+SHeaderRow::Column(StringTableKeyColumnId)
						.DefaultLabel(LOCTEXT("KeyColumnLabel", "Key"))
						.FillWidth(0.2f)

						+SHeaderRow::Column(StringTableSourceStringColumnId)
						.DefaultLabel(LOCTEXT("SourceStringColumnLabel", "Source String"))
						.FillWidth(1.0f)
				)
	];

}

void SUnrealDiffStringTableListView::Refresh()
{
	CachedStringTableEntries.Empty();
	SetupRowsListSources();
	MyListView->RequestListRefresh();
}

void SUnrealDiffStringTableListView::SetupRowsListSources()
{
	FStringTableConstRef StringTableRef = StringTableVisualDiff->GetStringTableRef(bIsLocal);
	StringTableRef->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
	{
		TSharedPtr<FCachedStringTableEntry> NewStringTableEntry = MakeShared<FCachedStringTableEntry>(InKey, InSourceString);
		CachedStringTableEntries.Add(NewStringTableEntry);

		return true; // continue enumeration
	});

	CachedStringTableEntries.Sort([](const TSharedPtr<FCachedStringTableEntry>& InEntryOne, const TSharedPtr<FCachedStringTableEntry>& InEntryTwo)
	{
		return InEntryOne->Key < InEntryTwo->Key;
	});

	
	for (auto& Entry : CachedStringTableEntries)
	{
		Entry->RowState = StringTableVisualDiff->GetRowState(bIsLocal, Entry->Key);
	}

	CachedStringTableEntries.RemoveAll([this](const TSharedPtr<FCachedStringTableEntry>& Entry)
	{
		if (StringTableVisualDiff->HasRowViewOption(static_cast<EUnrealVisualDiff::RowViewOption>(Entry->RowState)))
		{
			return false;
		}

		return true;
	});
}


TSharedRef<ITableRow> SUnrealDiffStringTableListView::OnGenerateStringTableEntryRow(TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry, const TSharedRef<STableViewBase>& Table)
{
	return SNew(SUnrealDiffStringTableEntryRow, Table, CachedStringTableEntry);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef  LOCTEXT_NAMESPACE
