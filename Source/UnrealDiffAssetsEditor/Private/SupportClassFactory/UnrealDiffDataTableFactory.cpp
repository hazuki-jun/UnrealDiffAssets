// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportClassFactory/UnrealDiffDataTableFactory.h"

#include "DataTableWidgets/SDataTableVisualDiff.h"

TSharedRef<SCompoundWidget> UUnrealDiffDataTableFactory::FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SDataTableVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
