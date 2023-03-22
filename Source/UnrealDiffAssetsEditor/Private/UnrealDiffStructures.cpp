// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealDiffStructures.h"

#include "GraphDiffControl.h"

void FMyGraphToDiff::BuildDiffSourceArray()
{
	FoundDiffs->Empty();
	FGraphDiffControl::DiffGraphs(GraphOld, GraphNew, *FoundDiffs);

	struct SortDiff
	{
		bool operator () (const FDiffSingleResult& A, const FDiffSingleResult& B) const
		{
			return A.Diff < B.Diff;
		}
	};

	Sort(FoundDiffs->GetData(), FoundDiffs->Num(), SortDiff());
}
