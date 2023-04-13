﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SVisualDiffWidget.h"
#include "UnrealDiffStringTableEntry.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SStringTableVisualDiff : public SVisualDiffWidget
{
public:
	SLATE_BEGIN_ARGS(SStringTableVisualDiff){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	void OnRowSelected(bool bIsLocal, FString RowKey);

	void Sync_HighlightRow(bool bIsLocal, FString RowKey);
	
	void InitSettings();
	
	//~ Begin SVisualDiffWidget Toolbar Action Interface
	// Next
	virtual void ToolbarAction_HighlightNextDifference() override;
	// Prev
	virtual void ToolbarAction_HighlightPrevDifference() override;
	// Diff
	virtual void ToolbarAction_Diff() override;
	virtual bool ToolbarAction_CanDiff() override;
	// Merge
	virtual void ToolbarAction_Merge() override;
	//~ End SVisualDiffWidget Toolbar Action Interface

	// Begin SVisualDiffWidget ViewOption Interface
	virtual void RefreshLayout() override;
	virtual void ModifyConfig() override;
	// End SVisualDiffWidget ViewOption Interface
	
	class UStringTable* GetStringTable(bool bIsLocal);
	FStringTableConstRef GetStringTableRef(bool bIsLocal);

	int32 GetRowState(bool bIsLocal, const FString& RowKey);
	
	void GetStringTableData(bool bIsLocal, TArray<TSharedPtr<FCachedStringTableEntry>>& OutEntries);

	void PerformMerge(const FString& RowKey);
	
protected:
	TSharedPtr<class SUnrealDiffStringTableListView> StringTableListViewLocal;
	TSharedPtr<class SUnrealDiffStringTableListView> StringTableListViewRemote;
	
protected:
	TSharedPtr<SWindow> ParentWindow;
	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;
};


