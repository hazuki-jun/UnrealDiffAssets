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
		SLATE_ARGUMENT( const class UBlueprint*, BlueprintOld )
		SLATE_ARGUMENT( const class UBlueprint*, BlueprintNew )
		SLATE_ARGUMENT( struct FRevisionInfo, OldRevision )
		SLATE_ARGUMENT( struct FRevisionInfo, NewRevision )
		SLATE_ARGUMENT( bool, ShowAssetNames )
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual ~SBlueprintVisualDiff() override;
};
