// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBlueprintDiff.h"

/**
 * 蓝图资源 对比 窗口
 */
class UNREALDIFFASSETSEDITOR_API SBlueprintVisualDiff : public SBlueprintDiff
{
public:
	SLATE_BEGIN_ARGS(SBlueprintVisualDiff){}
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void ConstructSuper();
	
	void OnTreeItemSelected(TSharedPtr<FBlueprintDifferenceTreeEntry> InTreeItem, ESelectInfo::Type Type);
	
	virtual ~SBlueprintVisualDiff() override;

	FReply OnMergeClicked();
	
	void OnActionMerge();

	void PerformMerge(TSharedPtr<TArray<FDiffSingleResult>> DiffResults, UEdGraph* LocalGraph, UEdGraph* RemoteGraph);

	void MergeFunctionGraph(UBlueprint* Blueprint, UEdGraph* LocalGraph, UEdGraph* RemoteGraph);
	
	void AddFunctionGraph(UBlueprint* Blueprint, class UEdGraph* Graph);

	void RemoveFunctionGraph(UBlueprint* Blueprint, class UEdGraph* InGraph);

	EVisibility GetMergeButtonVisibility() const;
	
protected:
	void Internal_GenerateDifferencesList();

	/** Called when user clicks on a new graph list item */
	void Internal_OnGraphSelectionChanged(TSharedPtr<struct FGraphToDiff> Item, ESelectInfo::Type SelectionType);

	void Internal_OnDiffListSelectionChanged(TSharedPtr<struct FDiffResultItem> TheDiff);
	
	void Internal_CreateGraphEntry(UEdGraph* GraphOld, UEdGraph* GraphNew);
	
protected:
	UObject* LocalAsset = nullptr;
	
	UObject* RemoteAsset = nullptr;

	TSharedPtr<SWindow> ParentWindow;

	FString SelectedGraphPath;

	EVisibility MergeButtonVisibility = EVisibility::Collapsed;
};






