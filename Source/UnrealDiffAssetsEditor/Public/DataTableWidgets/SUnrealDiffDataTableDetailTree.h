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
	void Construct(const FArguments& InArgs, class SUnrealDiffDetailView* DetailView_);

	TSharedRef<ITableRow> OnGenerateRowForDetailTree( TSharedPtr<class FUnrealDiffDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable );
	void OnGetChildrenForDetailTree( TSharedPtr<class FUnrealDiffDetailTreeNode> InTreeNode, TArray< TSharedPtr<class FUnrealDiffDetailTreeNode> >& OutChildren );
	
	TArray<TSharedPtr<class FUnrealDiffDetailTreeNode>> TreeNodes;

	void SetStructure(TSharedPtr<class FUnrealDiffStructOnScope> Structure);

	TMap<FName, TArray<FProperty*>> GetStructMembers(TSharedPtr<class FUnrealDiffStructOnScope> Structure);
	
	const uint8* GetPropertyData(UDataTable* DataTable, const FProperty* InProperty);
	
	class SUnrealDiffDetailView* DetailView;
	
protected:
	TSharedPtr<STreeView<TSharedPtr<class FUnrealDiffDetailTreeNode>>> MyTreeView;
};


