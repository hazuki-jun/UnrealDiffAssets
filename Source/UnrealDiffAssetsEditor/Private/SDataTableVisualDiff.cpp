// Fill out your copyright notice in the Description page of Project Settings.


#include "SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"
#include "Framework/Commands/GenericCommands.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDataTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		BuildWidgetContent()
	];

	UUnrealDiffAssetDelegate::OnDataTableRowSelected.BindRaw(this, &SDataTableVisualDiff::OnRowSelectionChanged);
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildWidgetContent()
{
	return SNew(SOverlay)
	+ SOverlay::Slot()
	[
		SNew(SSplitter)
#if ENGINE_MAJOR_VERSION == 4
		.Style(FEditorStyle::Get(), "DetailsView.Splitter")
#else
		.Style(FAppStyle::Get(), "DetailsView.Splitter")
#endif
		.PhysicalSplitterHandleSize(5.0f)
		.HitDetectionSplitterHandleSize(5.0f)
		+ SSplitter::Slot()
		[
			BuildLayoutWidget(FText::FromString(TEXT("DataTable Local")), LocalAsset, true)
		]

		+ SSplitter::Slot()
		[
			BuildLayoutWidget(FText::FromString(TEXT("DataTable Remote")), RemoteAsset, false)
		]
	];
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildLayoutWidget(FText InTitle, UObject* AssetObject, bool bIsLocal)
{
	return SNew(SUnrealDiffDataTableLayout).Title(InTitle).AssetObject(AssetObject).IsLocal(bIsLocal);
}

void SDataTableVisualDiff::OnRowSelectionChanged(bool bIsLocal, FName RowId)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowId;
}

void SDataTableVisualDiff::CopySelectedRow()
{
	if (SelectedRowId.IsNone())
	{
		return;
	}
	
	UDataTable* TablePtr = Cast<UDataTable>(bIsLocalDataTableSelected ? LocalAsset : RemoteAsset);
	uint8* RowPtr = TablePtr ? TablePtr->GetRowMap().FindRef(SelectedRowId) : nullptr;
	
	if (!RowPtr || !TablePtr->RowStruct)
	{
		SelectedRowId = NAME_None;
		return;
	}
	
	FString ClipboardValue;
	TablePtr->RowStruct->ExportText(ClipboardValue, RowPtr, RowPtr, TablePtr, PPF_Copy, nullptr);
	FPlatformApplicationMisc::ClipboardCopy(*ClipboardValue);
}

FReply SDataTableVisualDiff::OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		bPressedCtrl = true;
	}
	
	if (InKeyEvent.GetKey() == EKeys::C)
	{
		if (bPressedCtrl)
		{
			CopySelectedRow();
			bPressedCtrl = false;
		}
	}
	
	return SCompoundWidget::OnPreviewKeyDown(MyGeometry, InKeyEvent);
}

FReply SDataTableVisualDiff::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	bPressedCtrl = false;
	return SCompoundWidget::OnKeyUp(MyGeometry, InKeyEvent);
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
