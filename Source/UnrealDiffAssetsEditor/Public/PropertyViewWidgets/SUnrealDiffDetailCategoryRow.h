// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUnrealDiffDetailTableRowBase.h"
/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailCategoryRow : public SUnrealDiffDetailTableRowBase
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailCategoryRow) {}
		SLATE_ARGUMENT(FName, CategoryName)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

protected:
	const FSlateBrush* GetBackgroundImage() const;
	FSlateColor GetInnerBackgroundColor() const;
	FSlateColor GetOuterBackgroundColor() const;
};
