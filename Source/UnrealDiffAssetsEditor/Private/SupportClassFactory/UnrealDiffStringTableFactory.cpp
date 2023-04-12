// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportClassFactory/UnrealDiffStringTableFactory.h"

#include "StringTableWidgets/SStringTableVisualDiff.h"

TSharedRef<SWidget> UUnrealDiffStringTableFactory::FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SStringTableVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
