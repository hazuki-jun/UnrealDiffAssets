// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableWidgets/SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif
#include "UnrealDiffSaveGame.h"
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

	InitSettings();
	
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

void SDataTableVisualDiff::InitSettings()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultUnrealDiffAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/UnrealDiffAssetsEditor.UnrealDiffAssetSettings");


	bool Checked = true;
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyNormal"), Checked, FileName);
	Checked ? SetRowViewOption(EDataTableVisualDiff::Normal) : ClearRowViewOption(EDataTableVisualDiff::Normal);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyModify"), Checked, FileName);
	Checked ? SetRowViewOption(EDataTableVisualDiff::Modify) : ClearRowViewOption(EDataTableVisualDiff::Modify);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyAdded"), Checked, FileName);
	Checked ? SetRowViewOption(EDataTableVisualDiff::Added) : ClearRowViewOption(EDataTableVisualDiff::Added);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	Checked ? SetRowViewOption(EDataTableVisualDiff::Removed) : ClearRowViewOption(EDataTableVisualDiff::Removed);
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
		.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.Splitter")
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
	TSharedRef<SOverlay> Overlay = SNew(SOverlay);
	
	FToolBarBuilder DataTableVisualToolbarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	DataTableVisualToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SDataTableVisualDiff::ToolbarAction_HighlightPrevDifference))
		, NAME_None
		, LOCTEXT("PrevLabel", "Prev")
		, LOCTEXT("PrevTooltip", "Prev Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintMerge.PrevDiff")
	);

	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SDataTableVisualDiff::ToolbarAction_HighlightNextDifference))
		, NAME_None
		, LOCTEXT("Nextabel", "Next")
		, LOCTEXT("NextTooltip", "Next Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintMerge.NextDiff")
	);

	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SDataTableVisualDiff::ToolbarAction_Diff))
		, NAME_None
		, LOCTEXT("DiffLabel", "Diff")
		, LOCTEXT("DiffTooltip", "Diff")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.Actions.Diff")
	);
	
	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SDataTableVisualDiff::ToolbarAction_Merge))
		, NAME_None
		, LOCTEXT("MergeLabel", "Merge")
		, LOCTEXT("MergeTooltip", "Merge")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser")
	);
	
	Overlay->AddSlot()
	.HAlign(HAlign_Left)
	.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
	[
		DataTableVisualToolbarBuilder.MakeWidget()
	];

	// View option
	TSharedPtr<SLayeredImage> FilterImage =
		SNew(SLayeredImage)
#if ENGINE_MAJOR_VERSION == 4
		.Image(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("GenericViewButton"))
#else
		.Image(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("DetailsView.ViewOptions"))
#endif
		.ColorAndOpacity(FSlateColor::UseForeground());

	TSharedRef<SComboButton> ComboButton =
		SNew(SComboButton)
		.HasDownArrow(false)
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
		.ButtonStyle(FUnrealDiffWindowStyle::GetAppStyle(), "SimpleButton")
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
		.MenuContent()
		[
			GetShowViewOptionContent()
		]
		.ButtonContent()
		[
			FilterImage.ToSharedRef()
		];
	
	Overlay->AddSlot()
	.HAlign(HAlign_Center)
	.Padding(FMargin(0.f, 0.f, 20.f, 0.f))
	[
		ComboButton
	];
	
	return Overlay;
}

TSharedRef<SWidget> SDataTableVisualDiff::GetShowViewOptionContent()
{
	FMenuBuilder DetailViewOptions( true, nullptr);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyNormal", "Show Only Normal Rows"),
	LOCTEXT("ShowOnlyNormal_ToolTip", "Displays only row"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SDataTableVisualDiff::OnShowOnlyViewOptionClicked, EDataTableVisualDiff::Normal),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SDataTableVisualDiff::IsShowOnlyRowViewOptionChecked, EDataTableVisualDiff::Normal )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyModified", "Show Only Modified Rows"),
	LOCTEXT("ShowOnlyModified_ToolTip", "Displays only row which have been changed"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SDataTableVisualDiff::OnShowOnlyViewOptionClicked, EDataTableVisualDiff::Modify),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SDataTableVisualDiff::IsShowOnlyRowViewOptionChecked, EDataTableVisualDiff::Modify )
			),
		NAME_None,
		EUserInterfaceActionType::Check);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyAdded", "Show Only Added Rows"),
	LOCTEXT("ShowOnlyAdded_ToolTip", "Displays only row which added"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SDataTableVisualDiff::OnShowOnlyViewOptionClicked, EDataTableVisualDiff::Added),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SDataTableVisualDiff::IsShowOnlyRowViewOptionChecked, EDataTableVisualDiff::Added )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	// DetailViewOptions.AddMenuEntry( 
	// LOCTEXT("ShowOnlyRemoved", "Show Only Removed Rows"),
	// LOCTEXT("ShowOnlyRemoved_ToolTip", "Displays only row which have been removed"),
	// 	FSlateIcon(),
	// 	FUIAction(
	// 	FExecuteAction::CreateSP(this, &SDataTableVisualDiff::OnShowOnlyViewOptionClicked, EDataTableVisualDiff::Removed),
	// 	FCanExecuteAction(),
	// 	FIsActionChecked::CreateSP( this, &SDataTableVisualDiff::IsShowOnlyRowViewOptionChecked, EDataTableVisualDiff::Removed )
	// 		),
	// 	NAME_None,
	// 	EUserInterfaceActionType::Check);
	
	
	return DetailViewOptions.MakeWidget();
}

void SDataTableVisualDiff::OnShowOnlyViewOptionClicked(EDataTableVisualDiff::RowViewOption InViewOption)
{
	if (InViewOption == EDataTableVisualDiff::Added)
	{
		ReverseRowViewOption(EDataTableVisualDiff::Added);
		ReverseRowViewOption(EDataTableVisualDiff::Removed);
	}
	else
	{
		ReverseRowViewOption(InViewOption);
	}
	
	RefreshLayout();
	ModifyConfig();
}

bool SDataTableVisualDiff::HasRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption) const
{
	return (RowViewOption & InViewOption) != 0;
}

void SDataTableVisualDiff::ClearRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = RowViewOption & ~InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SDataTableVisualDiff::SetRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = RowViewOption | InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SDataTableVisualDiff::SetRowViewOptionTo(EDataTableVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = InViewOption;
}

void SDataTableVisualDiff::ReverseRowViewOption(EDataTableVisualDiff::RowViewOption InViewOption)
{
	HasRowViewOption(InViewOption) ? ClearRowViewOption(InViewOption) : SetRowViewOption(InViewOption);
}

void SDataTableVisualDiff::ModifyConfig()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultUnrealDiffAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/UnrealDiffAssetsEditor.UnrealDiffAssetSettings");
	
	bool Checked = HasRowViewOption(EDataTableVisualDiff::Normal);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyNormal"), Checked, FileName);
	
	Checked = HasRowViewOption(EDataTableVisualDiff::Modify);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyModify"), Checked, FileName);
	
	Checked = HasRowViewOption(EDataTableVisualDiff::Added);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyAdded"), Checked, FileName);
	
	Checked = HasRowViewOption(EDataTableVisualDiff::Removed);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	
	GConfig->Flush(false, FileName);
}

bool SDataTableVisualDiff::IsShowOnlyRowViewOptionChecked(EDataTableVisualDiff::RowViewOption InViewOption) const
{
	return HasRowViewOption(InViewOption);
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

void SDataTableVisualDiff::ToolbarAction_HighlightNextDifference()
{
	if (!DataTableLayoutLocal)
	{
		return;
	}

	CloseDetailView();
	
	auto& VisibleRows = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = 0; i < VisibleRows.Num(); ++i)
	{
		if (VisibleRows[i]->RowNum > SelectedRowNumber)
		{
			if (VisibleRows[i]->RowState > EDataTableVisualDiff::Normal)
			{
				DataTableLayoutLocal->SelectRow(VisibleRows[i]->RowId);
				OnRowSelectionChanged(true, VisibleRows[i]->RowId, VisibleRows[i]->RowNum);
				break;
			}
		}
	}
}

void SDataTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	if (!DataTableLayoutLocal)
	{
		return;
	}
	
	CloseDetailView();
	
	auto& VisibleRows = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = VisibleRows.Num() - 1; i >= 0; --i)
	{
		if (VisibleRows[i]->RowNum < SelectedRowNumber)
		{
			if (VisibleRows[i]->RowState > EDataTableVisualDiff::Normal)
			{
				DataTableLayoutLocal->SelectRow(VisibleRows[i]->RowId);
				OnRowSelectionChanged(true, VisibleRows[i]->RowId, VisibleRows[i]->RowNum);
				break;
			}
		}
	}
}

void SDataTableVisualDiff::ToolbarAction_Diff()
{
	if (!ToolbarAction_CanDiff())
	{
		return;
	}
	
	ShowDifference_RowToRow(SelectedRowId, SelectedRowNumber);
}

bool SDataTableVisualDiff::ToolbarAction_CanDiff()
{
	if (!DataTableLayoutLocal)
	{
		return false;
	}
	
	auto& VisibleRowsLocal = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = VisibleRowsLocal.Num() - 1; i >= 0; --i)
	{
		if (VisibleRowsLocal[i]->RowId.IsEqual(SelectedRowId))
		{
			if (VisibleRowsLocal[i]->RowState == EDataTableVisualDiff::Modify)
			{
				return true;
			}
		}
	}
	
	return false;
}

void SDataTableVisualDiff::ToolbarAction_Merge()
{
	auto& VisibleRows = DataTableLayoutRemote->GetVisibleRows();
	for (int32 i = VisibleRows.Num() - 1; i >= 0; --i)
	{
		if (VisibleRows[i]->RowId.IsEqual(SelectedRowId))
		{
			if (VisibleRows[i]->RowState == EDataTableVisualDiff::Removed)
			{
				MergeAction_DeleteRow(SelectedRowId);
				ToolbarAction_HighlightNextDifference();
				break;
			}
			else
			{
				MergeAction_MergeRow(SelectedRowId);
				ToolbarAction_HighlightNextDifference();
				break;
			}
		}
	}
}

void SDataTableVisualDiff::OnRowSelectionChanged(bool bIsLocal, FName RowId, int32 RowNumber)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowId;
	SelectedRowNumber = RowNumber;

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

void SDataTableVisualDiff::ShowDifference_RowToRow(const FName& RowName, int32 InSelectedRowNumber)
{
	if (!RowDetailViewLocal || !RowDetailViewRemote)
	{
		return;
	}

	SelectedRowId = RowName;
	SelectedRowNumber = InSelectedRowNumber;
	
	RowDetailViewLocal->Refresh(RowName);
	RowDetailViewRemote->Refresh(RowName);

	const auto& CacheNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& CacheNodesRemote = RowDetailViewRemote->GetCachedNodes();
	for (int32 i = 0; i < CacheNodesLocal.Num(); ++i)
	{
		if (CacheNodesRemote.IsValidIndex(i))
		{
			FString A = CacheNodesLocal[i]->ValueText.ToString();
			FString B = CacheNodesRemote[i]->ValueText.ToString();
			
			if (auto TextProp = CastField<FTextProperty>(CacheNodesRemote[i]->Property.Get()))
			{
				A = FUnrealDiffDataTableUtil::CopyProperty(CacheNodesLocal[i]);
				B = FUnrealDiffDataTableUtil::CopyProperty(CacheNodesRemote[i]);
			}
			
			if (!A.Equals(B))
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

	ExpandCategories();
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

void SDataTableVisualDiff::ExpandCategories()
{
	if (!RowDetailViewLocal || !RowDetailViewRemote)
	{
		return;
	}

	auto& AllNodes = RowDetailViewLocal->GetCachedNodes();
	for (const auto& TreeNode : AllNodes)
	{
		if (TreeNode->IsContainerNode())
		{
			if (UUnrealDiffSaveGame::IsRowCategoryExpanded(SelectedRowId, TreeNode->GetCategoryName()))
			{
				RowDetailViewLocal->SetItemExpansion(true, TreeNode->GetNodeIndex());
				RowDetailViewRemote->SetItemExpansion(true, TreeNode->GetNodeIndex());
			}
		}
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
		ShowDifference_RowToRow(SelectedRowId, SelectedRowNumber);
		ExpandCategories();
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
				RowData->RowState = 1;
			}
		}

		for (const auto& RowData : VisibleRowRemote)
		{
			if (RowData->RowId.IsEqual(SelectedRowId))
			{
				RowData->RowState = 1;
			}
		}
	}
}

void SDataTableVisualDiff::CloseDetailView()
{
	if (RowDetailViewLocal && RowDetailViewRemote)
	{
		if (RowDetailViewLocal->GetVisibility() != EVisibility::Collapsed)
		{
			RowDetailViewLocal->SetVisibility(EVisibility::Collapsed);
			RowDetailViewRemote->SetVisibility(EVisibility::Collapsed);	
		}
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

FSlateColor SDataTableVisualDiff::GetHeaderColumnColorAndOpacity(bool InIsLocal, int32 Index) const
{
	if (!DataTableLayoutLocal || ! DataTableLayoutRemote)
	{
		return FLinearColor(1.f, 1.f, 1.f);
	}

	auto& AvailableColumnsLocal = DataTableLayoutLocal->GetAvailableColumns();
	auto& AvailableColumnsRemote = DataTableLayoutRemote->GetAvailableColumns();

	auto ExistColumn = [Index](TArray<FDataTableEditorColumnHeaderDataPtr>& Dest, TArray<FDataTableEditorColumnHeaderDataPtr>& Src)
	{
		if (!Dest.IsValidIndex(Index))
		{
			return true;
		}

		FName Column =  Dest[Index]->ColumnId;
		for (const auto& ColumnHeaderData : Src)
		{
			if (ColumnHeaderData->ColumnId.IsEqual(Column))
			{
				return true;
			}
		}
		
		return false;	
	};

	bool bFound = false;
	if (InIsLocal)
	{
		bFound = ExistColumn(AvailableColumnsLocal, AvailableColumnsRemote);
	}
	else
	{
		bFound = ExistColumn(AvailableColumnsRemote, AvailableColumnsLocal);
	}
	
	return bFound ? FLinearColor(1.f, 1.f, 1.f) : FLinearColor(0.0, 0.8, 0.1, 1.0);
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
