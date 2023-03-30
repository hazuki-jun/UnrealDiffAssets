﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SUnrealDiffDetailTableRowBase.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDetailSingleItemRow : public SUnrealDiffDetailTableRowBase
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDetailSingleItemRow) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};