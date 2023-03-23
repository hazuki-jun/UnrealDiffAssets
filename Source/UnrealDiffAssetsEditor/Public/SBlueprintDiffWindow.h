// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

enum class EDiffAssetType
{
	None,
	Blueprint,
	DataTable,
};

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SBlueprintDiffWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SBlueprintDiffWindow) {}
		SLATE_ARGUMENT(FText, WindowTitel)
		SLATE_ARGUMENT(EDiffAssetType, DiffAssetType)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	TSharedRef<SCompoundWidget> GetBlueprintDiffWidget(UObject* LocalAsset, UObject* RemoteAsset);
	
	static TSharedPtr<SBlueprintDiffWindow> CreateWindow(EDiffAssetType AssetType, UObject* LocalAsset, UObject* RemoteAsset);
	
protected:
	EDiffAssetType DiffAssetType = EDiffAssetType::None;
};

