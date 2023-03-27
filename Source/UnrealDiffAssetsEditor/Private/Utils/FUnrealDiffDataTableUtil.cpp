#include "Utils/FUnrealDiffDataTableUtil.h"

#include "Engine/DataTable.h"
#include "DataTableEditorUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FUnrealDiffDataTableUtil"

void FUnrealDiffDataTableUtil::ExportRowText(FString& ValueStr, UDataTable* DataTable, const FName& RowName)
{
	uint8* RowPtr = DataTable ? DataTable->GetRowMap().FindRef(RowName) : nullptr;

	if (RowPtr)
	{
		DataTable->RowStruct->ExportText(ValueStr, RowPtr, RowPtr, DataTable, PPF_Copy, nullptr);
	}
}

void FUnrealDiffDataTableUtil::MergeRow(UDataTable* DestDataTable, UDataTable* SrcDataTable, const FName& RowName)
{
	if (!DestDataTable || !SrcDataTable || RowName.IsNone())
	{
		return;
	}

	FString SrcRow;
	ExportRowText(SrcRow, SrcDataTable, RowName);
	if (SrcRow.IsEmpty())
	{
		return;
	}

	if (!DestDataTable->GetRowMap().Find(RowName))
	{
		AddRow(DestDataTable, RowName);
	}
	
	uint8* RowPtr = DestDataTable ? DestDataTable->GetRowMap().FindRef(RowName) : nullptr;
	if (!RowPtr || !DestDataTable->RowStruct)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("PasteDataTableRow", "Paste Data Table Row"));
	DestDataTable->Modify();
	
	FDataTableEditorUtils::BroadcastPreChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	const TCHAR* Result = DestDataTable->RowStruct->ImportText(*SrcRow, RowPtr, DestDataTable, PPF_Copy, nullptr, GetPathNameSafe(DestDataTable->RowStruct));
	DestDataTable->HandleDataTableChanged(RowName);
	DestDataTable->MarkPackageDirty();
	FDataTableEditorUtils::BroadcastPostChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	
	if (Result == nullptr)
	{
		FNotificationInfo Info(LOCTEXT("FailedPaste", "Failed to paste row"));
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void FUnrealDiffDataTableUtil::AddRow(UDataTable* DestDataTable, const FName& RowName)
{
	FDataTableEditorUtils::AddRow(DestDataTable, RowName);
}

void FUnrealDiffDataTableUtil::DeleteRow(UDataTable* DataTable, FName RowName)
{
	FDataTableEditorUtils::RemoveRow(DataTable, RowName);
}

#undef LOCTEXT_NAMESPACE 
