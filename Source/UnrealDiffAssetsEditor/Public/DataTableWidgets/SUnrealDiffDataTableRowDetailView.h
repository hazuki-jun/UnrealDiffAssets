// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"
#include "PropertyViewWidgets/UnrealDiffDetailColumnSizeData.h"

class FUnrealDiffStructOnScope
{
public:
	class UDataTable* DataTable;
	FName CurrentRowName;
	TSharedPtr<class FStructOnScope> StructureData; 
};

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableRowDetailView : public SUnrealDiffDetailView
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableRowDetailView) {}
		SLATE_ARGUMENT(bool, IsLocal)
		SLATE_ARGUMENT(TSharedPtr<class SDataTableVisualDiff>, DataTableVisualDiff)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> BuildRowTitle();
	
	void Refresh(const FName& InRowName);

	//~ Begin SUnrealDiffDetailView Interface
	virtual FUnrealDiffDetailColumnSizeData& GetColumnSizeData() override { return DetailColumnSizeData; }
	virtual UDataTable* GetDataTable() override;
	virtual FName GetCurrentRowName() override;
	virtual SDataTableVisualDiff* GetDataTableVisualDiff() override;
	virtual bool IsLocalAsset() override { return bIsLocal; }
	//~ Begin SUnrealDiffDetailView Interface

	const FSlateBrush* GetCloseImage() const;
	FReply CloseButton_OnClicked();
	
	void SetItemExpansion(bool bIsExpand, int32 NodeIndex);
	void SetVerticalScrollOffset(float ScrollOffset);
	void RefreshWidgetFromItem(TSharedPtr<class FUnrealDiffDetailTreeNode> InItem);
	
protected:
	FUnrealDiffDetailColumnSizeData DetailColumnSizeData;
	
	bool bIsLocal = true;

	TSharedPtr<SButton> CloseButton;
	
	TSharedPtr<class SUnrealDiffDataTableDetailRowSelector> RowSelector;
	
	TSharedPtr<class SDataTableVisualDiff> DataTableVisualDiff;

	TSharedPtr<class SUnrealDiffDataTableDetailTree> MyDetailTree;
	
	FName RowName;
	
	FButtonStyle CloseButtonStyle;
};


