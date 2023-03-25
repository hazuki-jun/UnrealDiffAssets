// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "DataTableWidgets/SUnrealDiffDataTableListViewRow.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SDataTableVisualDiff : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDataTableVisualDiff){}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(UObject*, LocalAsset)
		SLATE_ARGUMENT(UObject*, RemoteAsset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> BuildWidgetContent();
	
	TSharedRef<SWidget> BuildLayoutWidget(FText InTitle, UObject* AssetObject, bool bIsLocal);

	void OnRowSelectionChanged(bool bIsLocal, FName RowId);
	
	void CopySelectedRow();

	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

public:
	//~ Cell
	void GetDataTableData(bool bIsLocal, TArray<FDataTableEditorColumnHeaderDataPtr> &OutAvailableColumns, TArray<FDataTableEditorRowListViewDataPtr> &OutAvailableRows);
	//~ Cell

	void SyncVerticalScrollOffset(bool bIsLocal, float NewOffset);

	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutLocal;
	TSharedPtr<class SUnrealDiffDataTableLayout> DataTableLayoutRemote;
	
protected:
	TSharedPtr<SWindow> ParentWindow;
	UObject* LocalAsset = nullptr;
	UObject* RemoteAsset = nullptr;
	bool bPressedCtrl = false;
	bool bIsLocalDataTableSelected = false;
	FName SelectedRowId;
};
