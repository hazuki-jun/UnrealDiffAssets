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
	void OnItemExpansionChanged(TSharedPtr<class FUnrealDiffDetailTreeNode> TreeItem, bool bIsExpanded) const;
	void HandleTableViewScrolled(double InScrollOffset);
	
	TArray<TSharedPtr<class FUnrealDiffDetailTreeNode>> RootTreeNodes;

	void SetStructure(TSharedPtr<class FUnrealDiffStructOnScope>& Structure);

	TMap<FName, TArray<FProperty*>> GetStructMembers(TSharedPtr<class FUnrealDiffStructOnScope>& Structure);
	
	class SUnrealDiffDetailView* DetailView = nullptr;

	const uint8* GetRowData(const FProperty* InProperty);
	
	void SetItemExpansion(bool bIsExpand, TSharedPtr<class FUnrealDiffDetailTreeNode> TreeItem);

	void SetVerticalScrollOffset(float ScrollOffset);

	void RefreshEachWidget(const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& InItems);
	
	void RefreshWidgetFromItem(const TSharedPtr<class FUnrealDiffDetailTreeNode>& InItem);
	
protected:
	TSharedPtr<STreeView<TSharedPtr<class FUnrealDiffDetailTreeNode>>> MyTreeView;
};






