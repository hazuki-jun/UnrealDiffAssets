// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealDiffSupportClassFactory.h"
#include "UObject/Object.h"
#include "UnrealDiffBlueprintFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffBlueprintFactory : public UUnrealDiffSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UUnrealDiffBlueprintFactory() { SupportClassName = TEXT("Blueprint"); }

	virtual TSharedRef<SCompoundWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffWidgetBlueprintFactory : public UUnrealDiffSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UUnrealDiffWidgetBlueprintFactory() { SupportClassName = TEXT("WidgetBlueprint"); }

	virtual TSharedRef<SCompoundWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffAnimationBlueprintFactory : public UUnrealDiffSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UUnrealDiffAnimationBlueprintFactory() { SupportClassName = TEXT("AnimBlueprint"); }

	virtual TSharedRef<SCompoundWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};
