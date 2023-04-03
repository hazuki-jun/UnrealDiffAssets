// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailView) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

public:
	virtual class FUnrealDiffDetailColumnSizeData& GetColumnSizeData() = 0;

	virtual UDataTable* GetDataTable() { return nullptr; }

	virtual FName GetCurrentRowName() { return NAME_None; }
};
