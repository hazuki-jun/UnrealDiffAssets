// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableDetailTree : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableDetailTree) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<ITableRow> OnGenerateRowForDetailTree( TSharedPtr<class UnrealDataTableDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable );
	
	TArray<TSharedPtr<class UnrealDataTableDetailTreeNode>> RootTreeNodes;

	void SetStructure(TSharedPtr<FStructOnScope> CurrentRow);
};
