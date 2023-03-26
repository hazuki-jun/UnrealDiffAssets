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
	WindowSize = ParentWindow->GetClientSizeInScreen();
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
	return SNew(SUnrealDiffDataTableLayout).Title(InTitle).IsLocal(bIsLocal).DataTableVisual(SharedThis(this));
}

void SDataTableVisualDiff::OnRowSelectionChanged(bool bIsLocal, FName RowId)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowId;

	if (bIsLocal)
	{
		if (DataTableLayoutRemote)
		{
			DataTableLayoutRemote->SelectRow(RowId);
		}
	}
	else
	{
		if (DataTableLayoutLocal)
		{
			DataTableLayoutLocal->SelectRow(RowId);
		}
	}
}

void SDataTableVisualDiff::CopyRow(bool bIsLocal, const FName& RowName)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowName;

	CopySelectedRow();
}

void SDataTableVisualDiff::CopySelectedRow()
{
	if (SelectedRowId.IsNone())
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
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
	ExportText(ClipboardValue, TablePtr, SelectedRowId);
	// TablePtr->RowStruct->ExportText(ClipboardValue, RowPtr, RowPtr, TablePtr, PPF_Copy, nullptr);
	FPlatformApplicationMisc::ClipboardCopy(*ClipboardValue);
}

void SDataTableVisualDiff::ExportText(FString& ValueStr, UDataTable* DataTable, FName& RowName) const
{
	uint8* RowPtr = DataTable ? DataTable->GetRowMap().FindRef(RowName) : nullptr;

	if (RowPtr)
	{
		DataTable->RowStruct->ExportText(ValueStr, RowPtr, RowPtr, DataTable, PPF_Copy, nullptr);
	}
}

void SDataTableVisualDiff::CopyRowName(const FName& RowName)
{
	if (SelectedRowId.IsNone())
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*RowName.ToString());
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


int32 SDataTableVisualDiff::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 Ret = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (ParentWindow.IsValid())
	{
		FVector2D ClientSize = ParentWindow->GetClientSizeInScreen();
		if (ClientSize != WindowSize)
		{
			WindowSize = ClientSize;
			UUnrealDiffAssetDelegate::OnWindowResized.Broadcast(WindowSize);
		}
	}
	
	return Ret;
}

void SDataTableVisualDiff::GetDataTableData(bool bIsLocal, TArray<FDataTableEditorColumnHeaderDataPtr>& OutAvailableColumns, TArray<FDataTableEditorRowListViewDataPtr>& OutAvailableRows)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	UDataTable* DataTableRemote = CastChecked<UDataTable>(RemoteAsset);

	const TMap<FName, uint8*>& RowMapLocal = DataTableLocal->GetRowMap();
	TArray<FName> RowNamesLocal = DataTableLocal->GetRowNames();
	TArray<FDataTableEditorRowListViewDataPtr> EmptyRowDataRemote;
	
	const TMap<FName, uint8*>& RowMapRemote = DataTableRemote->GetRowMap(); 
	TArray<FName> RowNamesRemote = DataTableRemote->GetRowNames();
	TArray<FDataTableEditorRowListViewDataPtr> EmptyRowDataLocal;
	
	if (bIsLocal)
	{
		FDataTableEditorUtils::CacheDataTableForEditing(DataTableLocal, OutAvailableColumns, OutAvailableRows);
		
		int32 Index = 0;
		for (auto RowIt = RowMapRemote.CreateConstIterator(); RowIt; ++RowIt, ++Index)
		{
			if (!RowMapLocal.Find(RowIt.Key()))
			{
				FDataTableEditorRowListViewDataPtr DataPtr = MakeShareable(new FDataTableEditorRowListViewData());
				DataPtr->RowId = RowIt->Key;
				DataPtr->RowNum = -2;
				OutAvailableRows.Add(DataPtr);
			}
		}

		for (auto& RowData : OutAvailableRows)
		{
			if (!RowMapRemote.Find(RowData->RowId))
			{
				RowData->RowNum = 0;
			}
			else if (RowData->RowNum > 0)
			{
				if (IsAnyDifferenceRowToTow(DataTableLocal, DataTableRemote, RowData->RowId))
				{
					RowData->RowNum = -1;
				}
			}
		}
	}
	else
	{
		FDataTableEditorUtils::CacheDataTableForEditing(DataTableRemote, OutAvailableColumns, OutAvailableRows);
		
		int32 Index = 0;
		for (auto RowIt = RowMapLocal.CreateConstIterator(); RowIt; ++RowIt, ++Index)
		{
			if (!RowMapRemote.Find(RowIt.Key()))
			{
				FDataTableEditorRowListViewDataPtr DataPtr = MakeShareable(new FDataTableEditorRowListViewData());
				DataPtr->RowId = RowIt->Key;
				DataPtr->RowNum = -2;
				OutAvailableRows.Add(DataPtr);
			}
		}

		for (auto& RowData : OutAvailableRows)
		{
			if (!RowMapLocal.Find(RowData->RowId))
			{
				RowData->RowNum = 0;
			}
			else if (RowData->RowNum > 0)
			{
				if (IsAnyDifferenceRowToTow(DataTableLocal, DataTableRemote, RowData->RowId))
				{
					RowData->RowNum = -1;
				}
			}
		}
	}
}

void SDataTableVisualDiff::SyncVerticalScrollOffset(bool bIsLocal, float NewOffset)
{
	if (bIsLocal)
	{
		if (DataTableLayoutRemote)
		{
			DataTableLayoutRemote->SetListViewScrollOffset(NewOffset);
		}
	}
	else
	{
		if (DataTableLayoutLocal)
		{
			DataTableLayoutLocal->SetListViewScrollOffset(NewOffset);
		}
	}
}

bool SDataTableVisualDiff::IsAnyDifferenceRowToTow(UDataTable* InLocalDataTable, UDataTable* InRemoteDataTable, FName& RowName)
{
	FString LocalStructDataText;
	ExportText(LocalStructDataText, InLocalDataTable, RowName);

	FString RemoteStructDataText;
	ExportText(RemoteStructDataText, InRemoteDataTable, RowName);

	if (LocalStructDataText.IsEmpty() || RemoteStructDataText.IsEmpty())
	{
		return false;
	}
				
	if (RemoteStructDataText.Len() != LocalStructDataText.Len())
	{
		return true;
	}
				
	if (!RemoteStructDataText.Equals(LocalStructDataText))
	{
		return true;
	}

	return false;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
