// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SBlueprintDiffWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SBlueprintDiffWindow) {}
		SLATE_ARGUMENT(FText, WindowTitel)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	TSharedRef<SWidget> GetBlueprintDiffWidget(UObject* LocalAsset, UObject* RemoteAsset);
	
	static TSharedPtr<SBlueprintDiffWindow> CreateWindow(UObject* LocalAsset, UObject* RemoteAsset);
};

