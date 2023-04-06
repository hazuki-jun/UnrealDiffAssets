// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif
#include "UnrealDiffWindowStyle.h"
#include "DataTableWidgets/SUnrealDiffDataTableLayout.h"
#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "Utils/FUnrealDiffDataTableUtil.h"
#include "Widgets/Images/SLayeredImage.h"
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
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference DataTable")));
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				// Toolbar
				MakeToolbar()
			]
			
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				BuildWidgetContent()
			]
		]
	];

	UUnrealDiffAssetDelegate::OnDataTableRowSelected.BindRaw(this, &SDataTableVisualDiff::OnRowSelectionChanged);
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildWidgetContent()
{
	TAttribute<float> RowDetailViewSplitterValue;
	RowDetailViewSplitterValue.BindRaw(this, &SDataTableVisualDiff::GetRowDetailViewSplitterValue);
	
	return SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SImage)
		.ColorAndOpacity(FSlateColor(FLinearColor(0.0, 0.0, 0.0, 1.0)))
	]
	
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
			SNew(SSplitter)
			.Orientation(EOrientation::Orient_Vertical)
			+ SSplitter::Slot()
			[
				BuildLayoutWidget(FText::FromString(FString::Format(TEXT("DataTable {0} [local]"), { *LocalAsset->GetName() })), true)
			]

			+ SSplitter::Slot()
			.Value(RowDetailViewSplitterValue)
			[
				BuildRowDetailView(true)
			]
		]

		+ SSplitter::Slot()
		[
			SNew(SSplitter)
			.Orientation(EOrientation::Orient_Vertical)
			+ SSplitter::Slot()
			[
				BuildLayoutWidget(FText::FromString(FString::Format(TEXT("DataTable {0} [Remote]"), { *RemoteAsset->GetName() })), false)
			]

			+ SSplitter::Slot()
			.Value(RowDetailViewSplitterValue)
			[
				BuildRowDetailView(false)
			]
		]
	];
}

float SDataTableVisualDiff::GetRowDetailViewSplitterValue() const
{
	if (RowDetailViewLocal && RowDetailViewLocal->GetVisibility() == EVisibility::SelfHitTestInvisible)
	{
		return 1.f;
	}
	
	return 0.f;
}

TSharedRef<SWidget> SDataTableVisualDiff::MakeToolbar()
{
	TSharedPtr<SLayeredImage> FilterImage = SNew(SLayeredImage)
		 .Image(FAppStyle::Get().GetBrush("DetailsView.ViewOptions"))
		 .ColorAndOpacity(FSlateColor::UseForeground());

	// Badge the filter icon if there are filters active
	// FilterImage->AddLayer(FUnrealDiffWindowStyle::GetAppSlateBrush("Icons.BadgeModified"));

	FMenuBuilder DetailViewOptions( true, nullptr);
	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyModified", "Show Only Modified Properties"),
	LOCTEXT("ShowOnlyModified_ToolTip", "Displays only properties which have been changed from their default"),
		FSlateIcon(),
		FUIAction(),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew( SComboButton )
			.HasDownArrow(false)
			.ContentPadding(0)
			.ForegroundColor( FSlateColor::UseForeground() )
			.ButtonStyle( FAppStyle::Get(), "SimpleButton" )
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
			.MenuContent()
			[
				DetailViewOptions.MakeWidget()
			]
			.ButtonContent()
			[
				FilterImage.ToSharedRef()
			]
		];
	
	// return SNew(SHorizontalBox)
	// + SHorizontalBox::Slot()
	// .AutoWidth()
	// [
	// 	SNew(SButton)
	// 	[
	// 		SNew(SHorizontalBox)
	// 		+ SHorizontalBox::Slot()
	// 		[
	// 			SNew(SImage)
	// 			.Image(FUnrealDiffWindowStyle::GetAppSlateBrush("BlueprintMerge.NextDiff"))
	// 		]
	// 		+ SHorizontalBox::Slot()
	// 		[
	// 			SNew(STextBlock)
	// 			.Text(FText::FromString(TEXT("Next ")))
	// 		]
	// 	]
	// ];
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildLayoutWidget(FText InTitle, bool bIsLocal)
{
	return SNew(SUnrealDiffDataTableLayout).Title(InTitle).IsLocal(bIsLocal).DataTableVisual(SharedThis(this));
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildRowDetailView(bool bIsLocal)
{
	if (bIsLocal)
	{
		SAssignNew(RowDetailViewLocal,SUnrealDiffDataTableRowDetailView)
		.Visibility(EVisibility::Collapsed)
		.IsLocal(bIsLocal)
		.DataTableVisualDiff(SharedThis(this));
		return RowDetailViewLocal.ToSharedRef();
	}
	else
	{
		SAssignNew(RowDetailViewRemote,SUnrealDiffDataTableRowDetailView)
		.Visibility(EVisibility::Collapsed)
		.IsLocal(bIsLocal)
		.DataTableVisualDiff(SharedThis(this));
		return RowDetailViewRemote.ToSharedRef();
	}
}

TSharedPtr<FStructOnScope> SDataTableVisualDiff::GetStructure()
{
	UDataTable* DataTable = Cast<UDataTable>(LocalAsset); 
	return MakeShareable(new FStructOnScope(DataTable->RowStruct));
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
	FUnrealDiffDataTableUtil::ExportRowText(ClipboardValue, TablePtr, SelectedRowId);
	FPlatformApplicationMisc::ClipboardCopy(*ClipboardValue);
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

void SDataTableVisualDiff::MergeAction_MergeRow(const FName& RowName)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	UDataTable* DataTableRemote = CastChecked<UDataTable>(RemoteAsset);
	FUnrealDiffDataTableUtil::MergeRow(DataTableLocal, DataTableRemote, RowName);

	RefreshLayout();
}

void SDataTableVisualDiff::MergeAction_DeleteRow(FName RowName)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	FUnrealDiffDataTableUtil::DeleteRow(DataTableLocal, RowName);

	RefreshLayout();
}

void SDataTableVisualDiff::ShowDifference_RowToRow(const FName& RowName)
{
	if (!RowDetailViewLocal || !RowDetailViewRemote)
	{
		return;
	}

	RowDetailViewLocal->Refresh(RowName);
	RowDetailViewRemote->Refresh(RowName);

	const auto& CacheNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& CacheNodesRemote = RowDetailViewRemote->GetCachedNodes();
	for (int32 i = 0; i < CacheNodesLocal.Num(); ++i)
	{
		if (CacheNodesRemote.IsValidIndex(i))
		{
			if (!CacheNodesRemote[i]->ValueText.ToString().Equals(CacheNodesLocal[i]->ValueText.ToString()))
			{
				CacheNodesRemote[i]->bHasAnyDifference = true;
				CacheNodesLocal[i]->bHasAnyDifference = true;
			}
		}
		else
		{
			return;
		}
	}
}

void SDataTableVisualDiff::RefreshLayout()
{
	if (!DataTableLayoutLocal || !DataTableLayoutRemote)
	{
		return;
	}

	DataTableLayoutLocal->Refresh();
	DataTableLayoutRemote->Refresh();

	if (RowDetailViewLocal && RowDetailViewRemote)
	{
		RowDetailViewLocal->SetVisibility(EVisibility::Collapsed);
		RowDetailViewRemote->SetVisibility(EVisibility::Collapsed);
	}
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

const uint8* SDataTableVisualDiff::GetPropertyData(const FProperty* InProperty)
{
	UDataTable* DataTable = Cast<UDataTable>(LocalAsset);
	if (!DataTable)
	{
		return nullptr;
	}

	auto RowMap = DataTable->GetRowMap();
	for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
	{
		for (TFieldIterator<FProperty> It(DataTable->RowStruct); It; ++It)
		{
			if (*It == InProperty)
			{
				return RowIt.Value();
			}
		}
	}
	
	return nullptr;
}

void SDataTableVisualDiff::SyncDetailViewAction_Expanded(bool bIsLocal, bool bIsExpanded, int32 NodeIndex)
{
	if (bIsLocal)
	{
		if (RowDetailViewRemote)
		{
			RowDetailViewRemote->SetItemExpansion(bIsExpanded, NodeIndex);
		}
	}
	else
	{
		if (RowDetailViewLocal)
		{
			RowDetailViewLocal->SetItemExpansion(bIsExpanded, NodeIndex);
		}
	}
}

void SDataTableVisualDiff::SyncDetailViewAction_VerticalScrollOffset(bool bIsLocal, float ScrollOffset)
{
	if (bIsLocal)
	{
		if (RowDetailViewRemote)
		{
			RowDetailViewRemote->SetVerticalScrollOffset(ScrollOffset);
		}
	}
	else
	{
		if (RowDetailViewLocal)
		{
			RowDetailViewLocal->SetVerticalScrollOffset(ScrollOffset);
		}
	}
}

void SDataTableVisualDiff::DetailViewAction_MergeProperty(int32 NodeIndex, const FString& PropertyValueString, bool bRegenerate)
{
	if (!RowDetailViewLocal)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("PasteDataTableRow", "Paste Data Table Row"));
	
	auto DestDataTable = Cast<UDataTable>(GetLocalAsset());
	DestDataTable->Modify();
	FDataTableEditorUtils::BroadcastPreChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	const auto& AllNodes = RowDetailViewLocal->GetCachedNodes();
	
	if (!AllNodes.IsValidIndex(NodeIndex))
	{
		return;
	}

	auto PropertyToModify = AllNodes[NodeIndex]->Property.Get();
	if (AllNodes[NodeIndex]->bIsInContainer)
	{
		void* ValueAddr = AllNodes[NodeIndex]->RowDataInContainer;
#if ENGINE_MAJOR_VERSION == 4
		PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
		PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
	}
	else
	{
		if (AllNodes[NodeIndex]->IsContainerNode())
		{
			void* ValueAddr = AllNodes[NodeIndex]->GetStructData(0);
#if ENGINE_MAJOR_VERSION == 4
			PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
			PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
		}
		else
		{
			void* StructData = AllNodes[NodeIndex]->GetStructData(0);
			void* ValueAddr = PropertyToModify->ContainerPtrToValuePtr<void>(StructData);
#if ENGINE_MAJOR_VERSION == 4
			PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
			PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
		}
	}

	DestDataTable->HandleDataTableChanged(SelectedRowId);
	DestDataTable->MarkPackageDirty();
	FDataTableEditorUtils::BroadcastPostChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);

	if (bRegenerate)
	{
		ShowDifference_RowToRow(SelectedRowId);
		if (RowDetailViewLocal && RowDetailViewRemote)
		{
			RowDetailViewLocal->SetItemExpansion(true, 0);
			RowDetailViewRemote->SetItemExpansion(true, 0);
		}
	}
	else
	{
		if (RowDetailViewLocal)
		{
			AllNodes[NodeIndex]->bHasAnyDifference = false;
			RowDetailViewLocal->RefreshWidgetFromItem(AllNodes[NodeIndex]);
		}
	}

	if (!FUnrealDiffDataTableUtil::HasAnyDifferenceRowToRow(GetLocalDataTable(), GetRemoteDataTable(), SelectedRowId))
	{
		if (!DataTableLayoutLocal || !DataTableLayoutRemote)
		{
			return;
		}

		auto& VisibleRowLocal = DataTableLayoutLocal->GetVisibleRows();
		auto& VisibleRowRemote = DataTableLayoutRemote->GetVisibleRows();
		for (const auto& RowData : VisibleRowLocal)
		{
			if (RowData->RowId.IsEqual(SelectedRowId))
			{
				RowData->bHasAnyDifference = false;
			}
		}

		for (const auto& RowData : VisibleRowRemote)
		{
			if (RowData->RowId.IsEqual(SelectedRowId))
			{
				RowData->bHasAnyDifference = false;
			}
		}
	}
}

void SDataTableVisualDiff::CloseDetailView()
{
	if (RowDetailViewLocal && RowDetailViewRemote)
	{
		RowDetailViewLocal->SetVisibility(EVisibility::Collapsed);
		RowDetailViewRemote->SetVisibility(EVisibility::Collapsed);
	}
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
				if (FUnrealDiffDataTableUtil::HasAnyDifferenceRowToRow(DataTableLocal, DataTableRemote, RowData->RowId))
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
				if (FUnrealDiffDataTableUtil::HasAnyDifferenceRowToRow(DataTableLocal, DataTableRemote, RowData->RowId))
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



END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
