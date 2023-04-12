// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealDiffSupportClassFactory.h"
#include "UnrealDiffStringTableFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffStringTableFactory : public UUnrealDiffSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UUnrealDiffStringTableFactory() { SupportClassName = TEXT("StringTable"); }

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};