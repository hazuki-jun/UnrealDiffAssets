// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IAssetTypeActions.h"

struct FMyGraphToDiff
{
	FMyGraphToDiff(class SBlueprintDiff* _DiffWidget, UEdGraph* _GraphOld, UEdGraph* _GraphNew, const FRevisionInfo& _RevisionOld, const FRevisionInfo& _RevisionNew)
		: DiffWidget(_DiffWidget)
		, GraphOld(_GraphOld)
		, GraphNew(_GraphNew)
		, RevisionOld(_RevisionOld)
		, RevisionNew(_RevisionNew)
		, FoundDiffs(MakeShared<TArray<FDiffSingleResult>>())
	{
		BuildDiffSourceArray();
	}

	/** Get old(left) graph*/
	UEdGraph* GetGraphOld() const { return GraphNew; } ;

	/** Get new(right) graph*/
	UEdGraph* GetGraphNew() const { return GraphOld; } ;
		
	/** Diff widget */
	class SBlueprintDiff* DiffWidget;

	/** The old graph(left)*/
	UEdGraph* GraphOld = nullptr;

	/** The new graph(right)*/
	UEdGraph* GraphNew = nullptr;

	/** Description of Old and new graph*/
	FRevisionInfo	RevisionOld, RevisionNew;

	TSharedPtr<TArray<FDiffSingleResult>> FoundDiffs;

	void BuildDiffSourceArray();
};
