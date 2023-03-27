// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBlueprintDiff.h"
#include "Widgets/SCompoundWidget.h"



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

	virtual ~SBlueprintVisualDiff() override;
	
	UObject* LocalAsset = nullptr;
	
	UObject* RemoteAsse = nullptr;
	
	void OnActionMerge();

	void PerformMerge(TSharedPtr<TArray<FDiffSingleResult>> DiffResults, UEdGraph* LocalGraph, UEdGraph* RemoteGraph);

	void MergeFunctionGraph(UBlueprint* Blueprint, UEdGraph* LocalGraph, UEdGraph* RemoteGraph);
	
	void AddFunctionGraph(UBlueprint* Blueprint, class UEdGraph* Graph);

	void RemoveFunctionGraph(UBlueprint* Blueprint, const FString& GraphPath);
};




