// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealDiffClipboardData.generated.h"

USTRUCT()
struct FUnrealDiffClipboardData
{
	GENERATED_BODY()
	FUnrealDiffClipboardData();
	FUnrealDiffClipboardData(const UEdGraph* InFuncGraph);
	
public:
	void SetFromGraph(const UEdGraph* InFuncGraph);
	
public:
	/** Name of the Graph */
	UPROPERTY()
	FName GraphName;

	/** The type of graph */
	UPROPERTY()
	TEnumAsByte<EGraphType> GraphType;

	/** The original Blueprint that this function was copied from. Used to determine if nodes need to be created as functions or custom events */
	UPROPERTY()
	TWeakObjectPtr<UBlueprint> OriginalBlueprint;

	/** A string containing the exported text for the nodes in this function */
	UPROPERTY()
	FString NodesString;
};
