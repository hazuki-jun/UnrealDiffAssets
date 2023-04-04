// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUnrealDiffDetailTableRowBase.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailRowIndent : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailRowIndent){}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<SUnrealDiffDetailTableRowBase> DetailsRow);

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	FOptionalSize GetIndentWidth() const;
	FSlateColor GetRowBackgroundColor(int32 IndentLevel) const;

private:
	TWeakPtr<SUnrealDiffDetailTableRowBase> Row;
};
