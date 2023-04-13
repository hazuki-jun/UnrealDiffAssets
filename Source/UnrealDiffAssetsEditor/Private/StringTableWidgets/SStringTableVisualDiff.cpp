// Fill out your copyright notice in the Description page of Project Settings.


#include "StringTableWidgets/SStringTableVisualDiff.h"

#include "SlateOptMacros.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "StringTableWidgets/SUnrealDiffStringTableListView.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SStringTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference StringTable")));
	InitSettings();
	
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		SNew(SOverlay)
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
	
}

void SStringTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	
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
	auto StringTableRefLocal = GetStringTableRef(true);
	auto StringTableRefRemote = GetStringTableRef(false);

	auto StringTableEntryLocal = StringTableRefLocal->FindEntry(RowKey);
	auto StringTableEntryRemote = StringTableRefRemote->FindEntry(RowKey);

	if (StringTableEntryLocal.IsValid() && StringTableEntryRemote.IsValid())
	{
		if (!StringTableEntryLocal->GetSourceString().Equals(StringTableEntryRemote->GetSourceString()))
		{
			return EUnrealVisualDiff::Modify;
		}
	}
	else if (bIsLocal && !StringTableEntryRemote.IsValid())
	{
		return EUnrealVisualDiff::Added;
	}
	else if (!bIsLocal && !StringTableEntryLocal.IsValid())
	{
		return EUnrealVisualDiff::Added;
	}

	return EUnrealVisualDiff::Normal;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

