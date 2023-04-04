// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffPropertyValueWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffPropertyValueWidget)
		{}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakPtr<class FUnrealDiffDetailTreeNode> InOwnerTreeNode);

	FText GetValueText(const FProperty* InProperty);

	FText GetValueTextInContainer(const FProperty* InProperty);

	FText GetValueTextFromStructData(const void* InStructData, const FProperty* InProperty);
	
	TWeakPtr<class FUnrealDiffDetailTreeNode> OwnerTreeNode;
};


