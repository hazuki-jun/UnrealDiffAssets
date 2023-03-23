// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBlueprintDiff.h"
#include "Widgets/SCompoundWidget.h"



/**
 * 
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

	UObject* LocalAsset = nullptr;
	
	UObject* RemoteAsse = nullptr;
	
	virtual ~SBlueprintVisualDiff() override;
	
	void OnActionMerge();

	void PerformMerge(TSharedPtr<TArray<FDiffSingleResult>> DiffResults, UEdGraph* LocalGraph, UEdGraph* RemoteGraph);

	void AddFunctionGraph(UBlueprint* Blueprint, class UEdGraph* Graph);

	void RemoveFunctionGraph(UBlueprint* Blueprint, const FString& GraphPath);
};




