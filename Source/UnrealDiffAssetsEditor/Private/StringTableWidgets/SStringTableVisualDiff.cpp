// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SStringTableVisualDiff.h"

#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "StringTableWidgets/SUnrealDiffStringTableListView.h"
#include "Utils/FUnrealDiffStringTableUtil.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SStringTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference StringTable")));
	InitSettings();

	UUnrealDiffAssetDelegate::OnStringTableRowSelected.BindRaw(this, &SStringTableVisualDiff::OnRowSelected);
	
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		SNew(SOverlay)
#if ENGINE_MAJOR_VERSION == 4
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f))
		]
#endif
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				MakeToolbar()
			]
			
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SSplitter)
				.PhysicalSplitterHandleSize(5.0f)
				.HitDetectionSplitterHandleSize(5.0f)
				.Orientation(EOrientation::Orient_Horizontal)
				+ SSplitter::Slot()
				[
					SAssignNew(StringTableListViewLocal, SUnrealDiffStringTableListView, SharedThis(this)).IsLocal(true)
				]

				+ SSplitter::Slot()
				[
					SAssignNew(StringTableListViewRemote, SUnrealDiffStringTableListView, SharedThis(this)).IsLocal(false)
				]
			]
		]
	];
}

void SStringTableVisualDiff::OnRowSelected(bool bIsLocal, FString RowKey, int32 RowNumber)
{
	SelectedRowKey = RowKey;
	SelectedRowNumber = RowNumber;
	Sync_HighlightRow(bIsLocal, RowKey);
}

void SStringTableVisualDiff::Sync_HighlightRow(bool bIsLocal, FString RowKey)
{
	if (!StringTableListViewLocal.IsValid() || !StringTableListViewRemote.IsValid())
	{
		return;
	}

	if (bIsLocal)
	{
		StringTableListViewRemote->HighlightRow(RowKey);
	}
	else
	{
		StringTableListViewLocal->HighlightRow(RowKey);
	}
}

void SStringTableVisualDiff::Sync_VerticalScrollOffset(bool bIsLocal, float ScrollOffset)
{
	if (!StringTableListViewLocal.IsValid() || !StringTableListViewRemote.IsValid())
	{
		return;
	}
	
	if (bIsLocal)
	{
		StringTableListViewRemote->SetScrollOffset(ScrollOffset);
	}
	// else
	// {
	// 	StringTableListViewLocal->SetScrollOffset(ScrollOffset);
	// }
}

void SStringTableVisualDiff::InitSettings()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultUnrealDiffAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/UnrealDiffAssetsEditor.UnrealDiffAssetSettings");
	
	bool Checked = true;
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyNormal"), Checked, FileName);
	Checked ? SetRowViewOption(EUnrealVisualDiff::Normal) : ClearRowViewOption(EUnrealVisualDiff::Normal);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyModify"), Checked, FileName);
	Checked ? SetRowViewOption(EUnrealVisualDiff::Modify) : ClearRowViewOption(EUnrealVisualDiff::Modify);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyAdded"), Checked, FileName);
	Checked ? SetRowViewOption(EUnrealVisualDiff::Added) : ClearRowViewOption(EUnrealVisualDiff::Added);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	Checked ? SetRowViewOption(EUnrealVisualDiff::Removed) : ClearRowViewOption(EUnrealVisualDiff::Removed);
}

void SStringTableVisualDiff::ToolbarAction_HighlightNextDifference()
{
	if (!StringTableListViewLocal.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<FCachedStringTableEntry>>& Entries = StringTableListViewLocal->CachedStringTableEntries;
	
	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		if (Entries[i]->RowNumber > SelectedRowNumber && Entries[i]->RowState != EUnrealVisualDiff::Normal)
		{
			OnRowSelected(true, Entries[i]->Key, Entries[i]->RowNumber);
			OnRowSelected(false, Entries[i]->Key, Entries[i]->RowNumber);
			break;
		}
	}
	
}

void SStringTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	if (!StringTableListViewLocal.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<FCachedStringTableEntry>>& Entries = StringTableListViewLocal->CachedStringTableEntries;
	
	for (int32 i = Entries.Num() - 1; i >= 0; --i)
	{
		if (Entries[i]->RowNumber < SelectedRowNumber && Entries[i]->RowState != EUnrealVisualDiff::Normal)
		{
			OnRowSelected(true, Entries[i]->Key, Entries[i]->RowNumber);
			OnRowSelected(false, Entries[i]->Key, Entries[i]->RowNumber);
			break;
		}
	}
}

void SStringTableVisualDiff::ToolbarAction_Diff()
{
	
}

bool SStringTableVisualDiff::ToolbarAction_CanDiff()
{
	return false;
}

void SStringTableVisualDiff::ToolbarAction_Merge()
{
	PerformMerge(SelectedRowKey);
	RefreshLayout();
}

void SStringTableVisualDiff::RefreshLayout()
{
	if (StringTableListViewLocal.IsValid() && StringTableListViewRemote.IsValid())
	{
		StringTableListViewLocal->Refresh();
		StringTableListViewRemote->Refresh();
	}
}

void SStringTableVisualDiff::ModifyConfig()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultUnrealDiffAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/UnrealDiffAssetsEditor.UnrealDiffAssetSettings");
	
	bool Checked = HasRowViewOption(EUnrealVisualDiff::Normal);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyNormal"), Checked, FileName);
	
	Checked = HasRowViewOption(EUnrealVisualDiff::Modify);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyModify"), Checked, FileName);
	
	Checked = HasRowViewOption(EUnrealVisualDiff::Added);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyAdded"), Checked, FileName);
	
	Checked = HasRowViewOption(EUnrealVisualDiff::Removed);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	
	GConfig->Flush(false, FileName);
}

UStringTable* SStringTableVisualDiff::GetStringTable(bool bIsLocal)
{
	return bIsLocal ? Cast<UStringTable>(LocalAsset) : Cast<UStringTable>(RemoteAsset);
}

FStringTableConstRef SStringTableVisualDiff::GetStringTableRef(bool bIsLocal)
{
	auto StringTable = GetStringTable(bIsLocal);
	check(StringTable);
	return StringTable->GetStringTable();
}

int32 SStringTableVisualDiff::GetRowState(bool bIsLocal, const FString& RowKey)
{
	if (bIsLocal)
	{
		for (const auto& Entry : StringTableListViewLocal->CachedStringTableEntries)
		{
			if (Entry->Key.Equals(RowKey))
			{
				return Entry->RowState;
			}
		}
	}
	else
	{
		for (const auto& Entry : StringTableListViewRemote->CachedStringTableEntries)
		{
			if (Entry->Key.Equals(RowKey))
			{
				return Entry->RowState;
			}
		}
	}
	
	return 1;
}

void SStringTableVisualDiff::GetStringTableData(bool bIsLocal, TArray<TSharedPtr<FCachedStringTableEntry>>& OutEntries)
{
	const auto StringTableRefLocal = GetStringTableRef(true);
	const auto StringTableRefRemote = GetStringTableRef(false);

	auto SetupEntries = [&OutEntries](const FStringTableConstRef& Dest, const FStringTableConstRef& Src)
	{
		Dest->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
		{
			TSharedPtr<FCachedStringTableEntry> NewStringTableEntry = MakeShared<FCachedStringTableEntry>(InKey, InSourceString);
			NewStringTableEntry->RowState = EUnrealVisualDiff::Normal;
			OutEntries.Add(NewStringTableEntry);

			return true; // continue enumeration
			
		});

		for (auto& Entry : OutEntries)
		{
			auto SrcEntry = Src->FindEntry(Entry->Key);
			if (!SrcEntry)
			{
				Entry->RowState = EUnrealVisualDiff::Added;
			}
			else
			{
				if (!Entry->SourceString.Equals(SrcEntry->GetSourceString()))
				{
					Entry->RowState = EUnrealVisualDiff::Modify;
				}
			}
		}

		Src->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
		{
			if (!Dest->FindEntry(InKey))
			{
				TSharedPtr<FCachedStringTableEntry> NewStringTableEntry = MakeShared<FCachedStringTableEntry>(InKey, FString());
				NewStringTableEntry->RowState = EUnrealVisualDiff::Removed;
				OutEntries.Add(NewStringTableEntry);			
			}
					
			return true; // continue enumeration
		});
	};
	
	if (bIsLocal)
	{
		SetupEntries(StringTableRefLocal, StringTableRefRemote);
	}
	else
	{
		SetupEntries(StringTableRefRemote, StringTableRefLocal);
	}
}

void SStringTableVisualDiff::PerformMerge(const FString& RowKey)
{
	EUnrealVisualDiff::RowViewOption RowState = static_cast<EUnrealVisualDiff::RowViewOption>(GetRowState(false, RowKey));  
	if (RowState == EUnrealVisualDiff::Removed)
	{
		FUnrealDiffStringTableUtil::DeleteRow(GetStringTable(true), RowKey); 
	}
	else if (RowState == EUnrealVisualDiff::Added)
	{
		auto StringTable = GetStringTableRef(false);
		auto Entry = StringTable->FindEntry(RowKey);
		if (Entry.IsValid())
		{
			FUnrealDiffStringTableUtil::AddRow(GetStringTable(true), RowKey, Entry->GetSourceString());
		}
	}
	else if (RowState == EUnrealVisualDiff::Modify)
	{
		auto StringTable = GetStringTableRef(false);
		auto Entry = StringTable->FindEntry(RowKey);
		if (Entry.IsValid())
		{
			FUnrealDiffStringTableUtil::ModifyRow(GetStringTable(true), RowKey, Entry->GetSourceString());
		}
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

