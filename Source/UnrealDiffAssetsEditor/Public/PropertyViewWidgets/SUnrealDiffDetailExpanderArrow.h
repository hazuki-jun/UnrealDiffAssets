// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailExpanderArrow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailExpanderArrow) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedPtr<class SUnrealDiffDetailTableRowBase> RowData);

	
private:

	EVisibility GetExpanderVisibility() const;
	const FSlateBrush* GetExpanderImage() const;
	FReply OnExpanderClicked();
	
private:
	TWeakPtr<class SUnrealDiffDetailTableRowBase> Row;
	TSharedPtr<SButton> ExpanderArrow;
};
