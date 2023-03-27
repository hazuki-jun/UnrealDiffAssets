// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealDiffSupportClassFactory.h"
#include "UnrealDiffDataTableFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffDataTableFactory : public UUnrealDiffSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UUnrealDiffDataTableFactory() { SupportClassName = TEXT("DataTable"); }

	virtual TSharedRef<SCompoundWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};
