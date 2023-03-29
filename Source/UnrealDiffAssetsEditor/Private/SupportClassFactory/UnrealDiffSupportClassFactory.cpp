// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportClassFactory/UnrealDiffSupportClassFactory.h"

UUnrealDiffSupportClassFactory::UUnrealDiffSupportClassFactory()
{
	
}

FName UUnrealDiffSupportClassFactory::GetSupportedClass()
{
	return SupportClassName;
}

TSharedRef<SWidget> UUnrealDiffSupportClassFactory::FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNullWidget::NullWidget;
}
