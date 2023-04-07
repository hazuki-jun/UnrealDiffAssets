// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */

class UNREALDIFFASSETSEDITOR_API UUnrealDiffAssetDelegate
{
public:
	DECLARE_DELEGATE(FOnBlueprintDiffWidgetClosed)
	static FOnBlueprintDiffWidgetClosed OnBlueprintDiffWidgetClosed;
	
	DECLARE_DELEGATE_ThreeParams(FOnDataTableRowSelected, bool, FName, int32)
	static FOnDataTableRowSelected OnDataTableRowSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnWindowResized, FVector2D)
	static FOnWindowResized OnWindowResized;
};
