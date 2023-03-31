// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailTableRowBase : public STableRow< TSharedPtr< class FUnrealDiffDetailTreeNode > >
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailTableRowBase) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual bool HasChildren() const { return false; }
};
