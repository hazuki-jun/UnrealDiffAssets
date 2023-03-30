// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UnrealDiffSupportClassFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffSupportClassFactory : public UObject
{
	GENERATED_BODY()

public:
	UUnrealDiffSupportClassFactory();
	
	virtual FName GetSupportedClass();

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset);
	
	FName SupportClassName;
};
