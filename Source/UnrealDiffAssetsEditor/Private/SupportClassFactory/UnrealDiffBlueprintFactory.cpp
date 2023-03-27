// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportClassFactory/UnrealDiffBlueprintFactory.h"

#include "BlueprintWidgets/SBlueprintVisualDiff.h"

TSharedRef<SCompoundWidget> UUnrealDiffBlueprintFactory::FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}

TSharedRef<SCompoundWidget> UUnrealDiffWidgetBlueprintFactory::FactoryCreateVisualWidget(
	TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}

TSharedRef<SCompoundWidget> UUnrealDiffAnimationBlueprintFactory::FactoryCreateVisualWidget(
	TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
