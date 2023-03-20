// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlueprintVisualDiff.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBlueprintVisualDiff::Construct(const FArguments& InArgs)
{
	SBlueprintDiff::FArguments Arguments;
	Arguments._BlueprintOld = InArgs._BlueprintOld;
	Arguments._BlueprintNew = InArgs._BlueprintNew;
	Arguments._OldRevision = InArgs._OldRevision;
	Arguments._NewRevision = InArgs._NewRevision;
	SBlueprintDiff::Construct(Arguments);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
