// Fill out your copyright notice in the Description page of Project Settings.


#include "SupportClassFactory/UnrealDiffDataTableFactory.h"
#include "Engine/DataTable.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"

bool UUnrealDiffDataTableFactory::CanDiff(UObject* InLocalAsset, UObject* InRemoteAsset)
{
	UDataTable* DataTableLocal = Cast<UDataTable>(InLocalAsset);
	UDataTable* DataTableRemote = Cast<UDataTable>(InRemoteAsset);

	if (DataTableLocal->GetRowStruct()->GetFName().IsEqual(DataTableRemote->GetRowStruct()->GetFName()))
	{
		return true;
	}
	
	return false;
}

FSuppressableWarningDialog::FSetupInfo UUnrealDiffDataTableFactory::GetDiffFailedDialogInfo()
{
	FSuppressableWarningDialog::FSetupInfo SetupInfo {FText::FromString(TEXT("表结构不一致")), FText::FromString(TEXT("Diff StringTable")), TEXT("NotSupport_Warning") };
	SetupInfo.ConfirmText = FText::FromString(TEXT("OK"));
	SetupInfo.CancelText = FText::FromString(TEXT("Cancel"));
	return SetupInfo;
}

TSharedRef<SWidget> UUnrealDiffDataTableFactory::FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SDataTableVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
