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
};
