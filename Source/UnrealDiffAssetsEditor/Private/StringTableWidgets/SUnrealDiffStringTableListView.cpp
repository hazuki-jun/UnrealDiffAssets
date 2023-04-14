// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SUnrealDiffStringTableListView.h"

#include "SlateOptMacros.h"
#include "Internationalization/StringTableCore.h"
#include "StringTableWidgets/SStringTableVisualDiff.h"
#include "StringTableWidgets/SUnrealDiffStringTableEntryRow.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffStringTableListView"

namespace NS_UnrealDiffStringTableListView
{
	const FName StringTableDummyColumnId("Dummy");
	const FName StringTableKeyColumnId("Key");
	const FName StringTableSourceStringColumnId("SourceString");
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffStringTableListView::Construct(const FArguments& InArgs, TSharedPtr<class SStringTableVisualDiff> InStringTableVisualDiff)
{
	StringTableVisualDiff = InStringTableVisualDiff;
	bIsLocal = InArgs._IsLocal;
	SetupRowsListSources();
	ChildSlot
	[
		SAssignNew(MyListView, SListView<TSharedPtr<FCachedStringTableEntry>>)
				.OnListViewScrolled(this, &SUnrealDiffStringTableListView::OnListViewScrolled)
				.ListItemsSource(&CachedStringTableEntries)
				.OnGenerateRow(this, &SUnrealDiffStringTableListView::OnGenerateStringTableEntryRow)
				.HeaderRow(
					SNew(SHeaderRow)
						+SHeaderRow::Column(NS_UnrealDiffStringTableListView::StringTableDummyColumnId)
						.DefaultLabel(FText::GetEmpty())
						.FixedWidth(20)

						+SHeaderRow::Column(NS_UnrealDiffStringTableListView::StringTableKeyColumnId)
						.DefaultLabel(LOCTEXT("KeyColumnLabel", "Key"))
						.FillWidth(0.2f)

						+SHeaderRow::Column(NS_UnrealDiffStringTableListView::StringTableSourceStringColumnId)
						.DefaultLabel(LOCTEXT("SourceStringColumnLabel", "Source String"))
						.FillWidth(1.0f)
				)
	];

}

void SUnrealDiffStringTableListView::OnListViewScrolled(double InScrollOffset)
{
	if (StringTableVisualDiff)
	{
		StringTableVisualDiff->Sync_VerticalScrollOffset(bIsLocal, InScrollOffset);
	}
}

void SUnrealDiffStringTableListView::Refresh()
{
	CachedStringTableEntries.Empty();
	SetupRowsListSources();
	MyListView->RequestListRefresh();
}

void SUnrealDiffStringTableListView::SetupRowsListSources()
{
	StringTableVisualDiff->GetStringTableData(bIsLocal, CachedStringTableEntries);
	
	CachedStringTableEntries.Sort([](const TSharedPtr<FCachedStringTableEntry>& InEntryOne, const TSharedPtr<FCachedStringTableEntry>& InEntryTwo)
	{
		return InEntryOne->Key < InEntryTwo->Key;
	});
	
	CachedStringTableEntries.RemoveAll([this](const TSharedPtr<FCachedStringTableEntry>& Entry)
	{
		if (StringTableVisualDiff->HasRowViewOption(static_cast<EUnrealVisualDiff::RowViewOption>(Entry->RowState)))
		{
			return false;
		}

		return true;
	});

	for (int32 i = 0; i < CachedStringTableEntries.Num(); ++i)
	{
		CachedStringTableEntries[i]->RowNumber = i + 1;
	}
}

void SUnrealDiffStringTableListView::HighlightRow(const FString& RowKey)
{
	for (const auto& Entry : CachedStringTableEntries)
	{
		if (Entry->Key.Equals(RowKey))
		{
			MyListView->SetItemSelection(Entry, true);
		}
		else
		{
			MyListView->SetItemSelection(Entry, false);
		}
	}
}

void SUnrealDiffStringTableListView::MergeRow(const FString& RowKey)
{
	if (StringTableVisualDiff.IsValid())
	{
		StringTableVisualDiff->PerformMerge(RowKey);
	}
}

void SUnrealDiffStringTableListView::SetScrollOffset(float InScrollOffset)
{
	if (MyListView.IsValid())
	{
		MyListView->SetScrollOffset(InScrollOffset);
	}
}


TSharedRef<ITableRow> SUnrealDiffStringTableListView::OnGenerateStringTableEntryRow(TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry, const TSharedRef<STableViewBase>& Table)
{
	return SNew(SUnrealDiffStringTableEntryRow, Table, CachedStringTableEntry, SharedThis(this)).IsLocal(bIsLocal);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
