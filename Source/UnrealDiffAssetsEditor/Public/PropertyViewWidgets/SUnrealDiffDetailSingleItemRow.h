// Fill out your copyright notice in the Description page of Project Settings.

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
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<class FUnrealDiffDetailTreeNode> InOwnerTreeNode);

	void Refresh();
	
	TSharedRef<SWidget> BuildRowContent();
	
	//~ Begin SWidget Interface
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~ End SWidget Interface

	TSharedRef<SWidget> MakeRowActionsMenu();

	/**
	 * @brief 粘贴选中Property到左侧
	 */
	void OnMenuActionMerge();

	/**
	 * @brief 拷贝选中Property的原始数据
	 */
	void OnMenuActionCopy();

	FString CopyProperty();
	
	//~ Begin SUnrealDiffDetailTableRowBase Interface
	virtual void OnExpanderClicked(bool bIsExpanded) override;

	virtual int32 GetIndentLevelForBackgroundColor() override;
	//~ End SUnrealDiffDetailTableRowBase Interface
	
public:
	TWeakPtr<class FUnrealDiffDetailTreeNode> OwnerTreeNode;

	TSharedPtr<class SUnrealDiffPropertyValueWidget> ValueWidget;
protected:
	FSlateColor GetOuterBackgroundColor() const;
	FSlateColor GetInnerBackgroundColor() const;
	bool IsHighlighted() const;
};
