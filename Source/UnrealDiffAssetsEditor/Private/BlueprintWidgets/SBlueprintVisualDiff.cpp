// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintWidgets/SBlueprintVisualDiff.h"

#include "EdGraphUtilities.h"
#include "FUDAGraphToDiff.h"
#include "GraphDiffControl.h"
#include "K2Node_FunctionEntry.h"
#include "SlateOptMacros.h"
#include "SMyBlueprint.h"

#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffClipboardData.h"
#include "UnrealDiffWindowStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

namespace
{
	static const TCHAR* VAR_PREFIX = TEXT("BPVar");
	static const TCHAR* GRAPH_PREFIX = TEXT("BPGraph");
}

#define STEAL_PROPERTY(Type, Class, Property) \
namespace NS_BlueprintVisualDiff\
{ \
	typedef Type Class::* Stolen_##Property##_Ptr; \
	template <Stolen_##Property##_Ptr Property> \
	struct Robber##Property \
	{ \
		friend Stolen_##Property##_Ptr Steal##Property() \
		{ \
			return Property; \
		} \
	}; \
	template struct Robber##Property<&Class::Property>; \
	Stolen_##Property##_Ptr Steal##Property(); \
} \


namespace NS_BlueprintVisualDiff
{
	static const FName BlueprintTypeMode = FName(TEXT("BlueprintTypeMode"));
	static const FName MyBlueprintMode = FName(TEXT("MyBlueprintMode"));
	static const FName DefaultsMode = FName(TEXT("DefaultsMode"));
	static const FName ClassSettingsMode = FName(TEXT("ClassSettingsMode"));
	static const FName ComponentsMode = FName(TEXT("ComponentsMode"));
	static const FName GraphMode = FName(TEXT("GraphMode"));
}

STEAL_PROPERTY(UEdGraph*, SMyBlueprint, EdGraph)
STEAL_PROPERTY(UEdGraph*, SGraphEditor, EdGraphObj)
STEAL_PROPERTY(UEdGraph*, FGraphToDiff, GraphOld)
STEAL_PROPERTY(UEdGraph*, FGraphToDiff, GraphNew)
// STEAL_PROPERTY(STreeView<TSharedPtr<FBlueprintDifferenceTreeEntry>>::FOnSelectionChanged, STreeView<TSharedPtr<FBlueprintDifferenceTreeEntry>>, OnSelectionChanged)
// 	bool bTreeItemsAreDirty = false;
#define LOCTEXT_NAMESPACE "SBlueprintVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBlueprintVisualDiff::Construct(const FArguments& InArgs)
{
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	ParentWindow = InArgs._ParentWindow;
	
	FMargin MergeButtonMargin(100.f, 48.0f, 0.0f, 0.f);
#if ENGINE_MAJOR_VERSION == 4
	MergeButtonMargin = FMargin(100.f, 40.0f, 0.0f, 0.f);
#endif
	
	InArgs._ParentWindow->SetTitle(FText::FromString(TEXT("Difference Blueprint")));

	ConstructSuper();
}

void SBlueprintVisualDiff::ConstructSuper()
{
	UBlueprint* OldBlueprint = CastChecked<UBlueprint>(LocalAsset);
	UBlueprint* NewBlueprint = CastChecked<UBlueprint>(RemoteAsset);
	
	PanelOld.Blueprint = OldBlueprint;
	PanelNew.Blueprint = NewBlueprint;
	PanelOld.RevisionInfo = FRevisionInfo { TEXT("Local") };
	PanelNew.RevisionInfo = FRevisionInfo { TEXT("Remote") };

	// Create a skeleton if we don't have one, this is true for revision history diffs
	if (!PanelOld.Blueprint->SkeletonGeneratedClass)
	{
		FKismetEditorUtilities::GenerateBlueprintSkeleton(const_cast<UBlueprint*>(PanelOld.Blueprint));
	}
	
	if (!PanelNew.Blueprint->SkeletonGeneratedClass)
	{
		FKismetEditorUtilities::GenerateBlueprintSkeleton(const_cast<UBlueprint*>(PanelNew.Blueprint));
	}

	// sometimes we want to clearly identify the assets being diffed (when it's
	// not the same asset in each panel)
	PanelOld.bShowAssetName = true;
	PanelNew.bShowAssetName = true;

	bLockViews = true;

	if (ParentWindow.IsValid())
	{
		WeakParentWindow = ParentWindow;

		AssetEditorCloseDelegate = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetEditorRequestClose().AddSP(this, &SBlueprintVisualDiff::OnCloseAssetEditor);
	}

	FToolBarBuilder NavToolBarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	NavToolBarBuilder.AddToolBarButton(
		FUIAction(
			FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::PrevDiff),
			FCanExecuteAction::CreateSP( this, &SBlueprintVisualDiff::HasPrevDiff)
		)
		, NAME_None
		, LOCTEXT("PrevDiffLabel", "Prev")
		, LOCTEXT("PrevDiffTooltip", "Go to previous difference")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintDif.PrevDiff")
	);
	NavToolBarBuilder.AddToolBarButton(
		FUIAction(
			FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::NextDiff),
			FCanExecuteAction::CreateSP(this, &SBlueprintVisualDiff::HasNextDiff)
		)
		, NAME_None
		, LOCTEXT("NextDiffLabel", "Next")
		, LOCTEXT("NextDiffTooltip", "Go to next difference")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintDif.NextDiff")
	);

	NavToolBarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::OnActionMerge))
		, NAME_None
		, LOCTEXT("UseSelectedLabel", "Merge")
		, LOCTEXT("UseSelectedTooltip", "Use Selected Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser")
	);
	
	FToolBarBuilder GraphToolbarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	GraphToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::OnToggleLockView))
		, NAME_None
		, LOCTEXT("LockGraphsLabel", "Lock/Unlock")
		, LOCTEXT("LockGraphsTooltip", "Force all graph views to change together, or allow independent scrolling/zooming")
		, TAttribute<FSlateIcon>(this, &SBlueprintVisualDiff::GetLockViewImage)
	);
	GraphToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::OnToggleSplitViewMode))
		, NAME_None
		, LOCTEXT("SplitGraphsModeLabel", "Vertical/Horizontal")
		, LOCTEXT("SplitGraphsModeLabelTooltip", "Toggles the split view of graphs between vertical and horizontal")
		, TAttribute<FSlateIcon>(this, &SBlueprintVisualDiff::GetSplitViewModeImage)
	);


	const auto RowGenerator = [](TSharedPtr< FBlueprintDifferenceTreeEntry > Entry, const TSharedRef<STableViewBase>& Owner) -> TSharedRef< ITableRow >
	{
		return SNew(STableRow<TSharedPtr<FBlueprintDifferenceTreeEntry> >, Owner)
			[
				Entry->GenerateWidget.Execute()
			];
	};

	const auto ChildrenAccessor = [](TSharedPtr<FBlueprintDifferenceTreeEntry> InTreeItem, TArray< TSharedPtr< FBlueprintDifferenceTreeEntry > >& OutChildren)
	{
		OutChildren = InTreeItem->Children;
	};

	const auto Selector = [this](TSharedPtr<FBlueprintDifferenceTreeEntry> InTreeItem, ESelectInfo::Type Type)
	{
		if (InTreeItem.IsValid())
		{
			InTreeItem->OnFocus.ExecuteIfBound();
		}
	};

	DifferencesTreeView = SNew(STreeView< TSharedPtr< FBlueprintDifferenceTreeEntry > >)
		.OnGenerateRow(STreeView< TSharedPtr< FBlueprintDifferenceTreeEntry > >::FOnGenerateRow::CreateStatic(RowGenerator))
		.OnGetChildren(STreeView< TSharedPtr< FBlueprintDifferenceTreeEntry > >::FOnGetChildren::CreateStatic(ChildrenAccessor))
		.OnSelectionChanged(STreeView< TSharedPtr< FBlueprintDifferenceTreeEntry > >::FOnSelectionChanged::CreateRaw(this, &SBlueprintVisualDiff::OnTreeItemSelected))
		.TreeItemsSource(&PrimaryDifferencesList);
	
	
	// DifferencesTreeView = DiffTreeView::CreateTreeView(&PrimaryDifferencesList);

	GenerateDifferencesList();
	
	const auto TextBlock = [](FText Text) -> TSharedRef<SWidget>
	{
		return SNew(SBox)
		.Padding(FMargin(4.0f,10.0f))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Visibility(EVisibility::HitTestInvisible)
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Text(Text)
		];
	};

	TopRevisionInfoWidget =
		SNew(SSplitter)
		.Visibility(EVisibility::HitTestInvisible)
		+ SSplitter::Slot()
		.Value(.2f)
		[
			SNew(SBox)
		]
		+ SSplitter::Slot()
		.Value(.8f)
		[
			SNew(SSplitter)
			.PhysicalSplitterHandleSize(10.0f)
			+ SSplitter::Slot()
			.Value(.5f)
			[
				TextBlock(DiffViewUtils::GetPanelLabel(PanelOld.Blueprint, PanelOld.RevisionInfo, FText()))
			]
			+ SSplitter::Slot()
			.Value(.5f)
			[
				TextBlock(DiffViewUtils::GetPanelLabel(PanelNew.Blueprint, PanelNew.RevisionInfo, FText()))
			]
		];

	GraphToolBarWidget = 
		SNew(SSplitter)
		.Visibility(EVisibility::HitTestInvisible)
		+ SSplitter::Slot()
		.Value(.2f)
		[
			SNew(SBox)
		]
		+ SSplitter::Slot()
		.Value(.8f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				GraphToolbarBuilder.MakeWidget()
			]	
		];

	this->ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush( "Docking.Tab", ".ContentAreaBrush" ))
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.VAlign(VAlign_Top)
				[
					TopRevisionInfoWidget.ToSharedRef()		
				]
				+ SOverlay::Slot()
				.VAlign(VAlign_Top)
				.Padding(0.0f, 6.0f, 0.0f, 4.0f)
				[
					GraphToolBarWidget.ToSharedRef()
				]
				+ SOverlay::Slot()
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f, 0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(4.f)
						.AutoWidth()
						[
							NavToolBarBuilder.MakeWidget()
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SSpacer)
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SSplitter)
						+ SSplitter::Slot()
						.Value(.2f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							[
								DifferencesTreeView.ToSharedRef()
							]
						]
						+ SSplitter::Slot()
						.Value(.8f)
						[
							SAssignNew(ModeContents, SBox)
						]
					]
				]
			]
		];

	SetCurrentMode(NS_BlueprintVisualDiff::MyBlueprintMode);

	// Bind to blueprint changed events as they may be real in memory blueprints that will be modified
	const_cast<UBlueprint*>(PanelNew.Blueprint)->OnChanged().AddSP(this, &SBlueprintDiff::OnBlueprintChanged);
	const_cast<UBlueprint*>(PanelOld.Blueprint)->OnChanged().AddSP(this, &SBlueprintDiff::OnBlueprintChanged);
}

void SBlueprintVisualDiff::OnTreeItemSelected(TSharedPtr<FBlueprintDifferenceTreeEntry> InTreeItem, ESelectInfo::Type Type)
{
	if (!InTreeItem.IsValid())
	{
		return;
	}

	InTreeItem->OnFocus.ExecuteIfBound();
}

void SBlueprintVisualDiff::OnActionMerge()
{
	UEdGraph* GraphOld = nullptr;
	UEdGraph* GraphNew = nullptr;
	TSharedPtr<TArray<FDiffSingleResult>> DiffResults;
	int32 RealDifferencesStartIndex = INDEX_NONE;
	auto GraphOldPtr = NS_BlueprintVisualDiff::StealGraphOld();
	auto GraphNewPtr = NS_BlueprintVisualDiff::StealGraphNew();
	for (const TSharedPtr<FGraphToDiff>& GraphToDiff : Graphs)
	{
		UEdGraph* NewGraph = GraphToDiff.Get()->*GraphOldPtr;
		UEdGraph* OldGraph = GraphToDiff.Get()->*GraphNewPtr;
		
		const FString OtherGraphPath = NewGraph ? FGraphDiffControl::GetGraphPath(NewGraph) : FGraphDiffControl::GetGraphPath(OldGraph);
		if (SelectedGraphPath.Equals(OtherGraphPath))
		{
			GraphNew = NewGraph;
			GraphOld = OldGraph;
			DiffResults = GraphToDiff->FoundDiffs;
			RealDifferencesStartIndex = GraphToDiff->RealDifferencesStartIndex;
			break;
		}
	}

	PerformMerge(DiffResults, GraphOld, GraphNew);
	
	// UEdGraph* LocalGraph = nullptr;
	// UEdGraph* RemoteGraph = nullptr;
	//
	// auto StolenEdGraphPtr = ::StealEdGraph();
	// LocalGraph = PanelOld.MyBlueprint.Get()->*StolenEdGraphPtr;
	// RemoteGraph = PanelNew.MyBlueprint.Get()->*StolenEdGraphPtr;
	//
	// if (!LocalGraph && !RemoteGraph)
	// {
	// 	return;
	// }

	// if (LocalGraph)
	// {
	// 	const FString LocalEdGraphPath = FGraphDiffControl::GetGraphPath(LocalGraph);
	// 	const FString EditorEdGraphPath = FGraphDiffControl::GetGraphPath(RemoteGraph);
	// 	if (!LocalEdGraphPath.Equals(EditorEdGraphPath))
	// 	{
	// 		LocalGraph = nullptr;
	// 	}
	// }
	//
	// TSharedPtr<TArray<FDiffSingleResult>> DiffResults = MakeShared<TArray<FDiffSingleResult>>();
	// FGraphDiffControl::DiffGraphs(LocalGraph, RemoteGraph, *DiffResults);
	// struct SortDiff
	// {
	// 	bool operator () (const FDiffSingleResult& A, const FDiffSingleResult& B) const
	// 	{
	// 		return A.Diff < B.Diff;
	// 	}
	// };
	// Sort(DiffResults->GetData(), DiffResults->Num(), SortDiff());
	// PerformMerge(DiffResults, LocalGraph, RemoteGraph);
}

void SBlueprintVisualDiff::PerformMerge(TSharedPtr<TArray<FDiffSingleResult>> DiffResults, UEdGraph* LocalGraph, UEdGraph* RemoteGraph)
{
	UBlueprint* LocalAssetBlueprint = CastChecked<UBlueprint>(LocalAsset);
	
	if (DiffResults->Num() > 0)
	{
		// 对比的是事件
		
	}
	else
	{
		// 对比的是函数
		// MergeFunctionGraph(LocalAssetBlueprint, LocalGraph, RemoteGraph);
	}
}

void SBlueprintVisualDiff::MergeFunctionGraph(UBlueprint* Blueprint, UEdGraph* LocalGraph, UEdGraph* RemoteGraph)
{
	FString FocusGraphPath;
	if (RemoteGraph)
	{
		FocusGraphPath = FGraphDiffControl::GetGraphPath(RemoteGraph);
	}
	
	if (!LocalGraph)
	{
		// 本地蓝图不存在函数 FocusGraphPath
		AddFunctionGraph(Blueprint, RemoteGraph);
	}
	else if (!RemoteGraph)
	{
		// 对比的蓝图不存在 FocusGraphPath
		RemoveFunctionGraph(Blueprint, FocusGraphPath);
	}
}

void SBlueprintVisualDiff::AddFunctionGraph(UBlueprint* Blueprint, UEdGraph* Graph)
{
	FString OutputString;
	FUnrealDiffClipboardData ClipboardData(Graph);
	ClipboardData.StaticStruct()->ExportText(OutputString, &ClipboardData, &ClipboardData, nullptr, 0, nullptr, false);

	if (!OutputString.IsEmpty())
	{
		OutputString = ::GRAPH_PREFIX + OutputString;;
		
		FPlatformApplicationMisc::ClipboardCopy(OutputString.GetCharArray().GetData());
	}

	FUnrealDiffClipboardData FuncData;
	FStringOutputDevice Errors;
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);
	const TCHAR* Import = ClipboardText.GetCharArray().GetData() + FCString::Strlen(::GRAPH_PREFIX);
	FUnrealDiffClipboardData::StaticStruct()->ImportText(Import, &FuncData, nullptr, 0, &Errors, FUnrealDiffClipboardData::StaticStruct()->GetName());
	
	FString FunctionName = FGraphDiffControl::GetGraphPath(Graph);
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FName(*FunctionName), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(NewGraph, FuncData.NodesString, PastedNodes);
	
	Blueprint->FunctionGraphs.Add(NewGraph);

	TArray<UK2Node_FunctionEntry*> Entry;
	NewGraph->GetNodesOfClass<UK2Node_FunctionEntry>(Entry);
	if (ensure(Entry.Num() == 1))
	{
		TArray<UK2Node_FunctionEntry*> OldEntry;
		Graph->GetNodesOfClass<UK2Node_FunctionEntry>(OldEntry);
		if (OldEntry.Num() == 1)
		{
			// Discard category
			Entry[0]->MetaData.Category = OldEntry[0]->MetaData.Category;
		}
		else
		{
			Entry[0]->MetaData.Category = UEdGraphSchema_K2::VR_DefaultCategory;
		}

		// Add necessary function flags
		int32 AdditionalFunctionFlags = (FUNC_BlueprintEvent | FUNC_BlueprintCallable);
		if ((Entry[0]->GetExtraFlags() & FUNC_AccessSpecifiers) == FUNC_None)
		{
			AdditionalFunctionFlags |= FUNC_Public;
		}
		Entry[0]->AddExtraFlags(AdditionalFunctionFlags);

		Entry[0]->FunctionReference.SetExternalMember(Graph->GetFName(), nullptr);
	}
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}

void SBlueprintVisualDiff::RemoveFunctionGraph(UBlueprint* Blueprint, const FString& GraphPath)
{
	// for (const auto& FunctionGraph : LocalAssetBlueprint->FunctionGraphs)
	// {
	// 	const FString OtherGraphPath = FGraphDiffControl::GetGraphPath(FunctionGraph);
	// 	if (FocusGraphPath.Equals(OtherGraphPath))
	// 	{
	// 		FBlueprintEditorUtils::RemoveGraph(LocalAssetBlueprint, FunctionGraph);
	// 		break;
	// 	}
	// }
}

void SBlueprintVisualDiff::GenerateDifferencesList()
{
	PrimaryDifferencesList.Empty();
	RealDifferences.Empty();
	Graphs.Empty();
	ModePanels.Empty();

	// SMyBlueprint needs to be created *before* the KismetInspector or the diffs are generated, because the KismetInspector's customizations
	// need a reference to the SMyBlueprint widget that is controlling them...
	const auto CreateInspector = [](TSharedPtr<SMyBlueprint> InMyBlueprint) {
		return SNew(SKismetInspector)
			.HideNameArea(true)
			.ViewIdentifier(FName("BlueprintInspector"))
			.MyBlueprintWidget(InMyBlueprint)
			.IsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateStatic([] { return false; }))
			.ShowLocalVariables(true);
	};

	PanelOld.GenerateMyBlueprintWidget();
	PanelOld.DetailsView = CreateInspector(PanelOld.MyBlueprint);
	PanelOld.MyBlueprint->SetInspector(PanelOld.DetailsView);
	PanelNew.GenerateMyBlueprintWidget();
	PanelNew.DetailsView = CreateInspector(PanelNew.MyBlueprint);
	PanelNew.MyBlueprint->SetInspector(PanelNew.DetailsView);

	TArray<UEdGraph*> GraphsOld, GraphsNew;
	PanelOld.Blueprint->GetAllGraphs(GraphsOld);
	PanelNew.Blueprint->GetAllGraphs(GraphsNew);

	//Add Graphs that exist in both blueprints, or in blueprint 1 only
	for (UEdGraph* GraphOld : GraphsOld)
	{
		UEdGraph* GraphNew = nullptr;
		for (UEdGraph*& TestGraph : GraphsNew)
		{
			if (TestGraph && GraphOld->GetName() == TestGraph->GetName())
			{
				GraphNew = TestGraph;

				// Null reference inside array
				TestGraph = nullptr;
				break;
			}
		}
		// Do not worry about graphs that are contained in MathExpression nodes, they are recreated each compile
		if (IsGraphDiffNeeded(GraphOld))
		{
			Internal_CreateGraphEntry(GraphOld,GraphNew);
		}
	}

	//Add graphs that only exist in 2nd(new) blueprint
	for (UEdGraph* GraphNew : GraphsNew)
	{
		if (GraphNew != nullptr && IsGraphDiffNeeded(GraphNew))
		{
			Internal_CreateGraphEntry(nullptr, GraphNew);
		}
	}

	bool bHasComponents = false;
	UClass* BlueprintClassOld = PanelOld.Blueprint->GeneratedClass;
	UClass* BlueprintClassNew = PanelNew.Blueprint->GeneratedClass;
	const bool bIsOldClassActor = BlueprintClassOld && BlueprintClassOld->IsChildOf<AActor>();
	const bool bIsNewClassActor = BlueprintClassNew && BlueprintClassNew->IsChildOf<AActor>();
	if (bIsOldClassActor || bIsNewClassActor)
	{
		bHasComponents = true;
	}

	// If this isn't a normal blueprint type, add the type panel
	if (PanelOld.Blueprint->GetClass() != UBlueprint::StaticClass())
	{
		ModePanels.Add(NS_BlueprintVisualDiff::BlueprintTypeMode, GenerateBlueprintTypePanel());
	}

	// Now that we have done the diffs, create the panel widgets
	ModePanels.Add(NS_BlueprintVisualDiff::MyBlueprintMode, GenerateMyBlueprintPanel());
	ModePanels.Add(NS_BlueprintVisualDiff::GraphMode, GenerateGraphPanel());
	ModePanels.Add(NS_BlueprintVisualDiff::DefaultsMode, GenerateDefaultsPanel());
	ModePanels.Add(NS_BlueprintVisualDiff::ClassSettingsMode, GenerateClassSettingsPanel());
	if (bHasComponents)
	{
		ModePanels.Add(NS_BlueprintVisualDiff::ComponentsMode, GenerateComponentsPanel());
	}

	for (const TSharedPtr<FGraphToDiff>& Graph : Graphs)
	{
		TArray<TSharedPtr<FBlueprintDifferenceTreeEntry>> TempTreeEntries;
		Graph->GenerateTreeEntries(TempTreeEntries, RealDifferences);
		for (const auto& TreeEntry : TempTreeEntries)
		{
			TreeEntry->OnFocus.Unbind();
			TreeEntry->OnFocus.BindRaw(this, &SBlueprintVisualDiff::Internal_OnGraphSelectionChanged, Graph, ESelectInfo::Direct);
		}
		PrimaryDifferencesList.Append(MoveTemp(TempTreeEntries));
	}

	DifferencesTreeView->RebuildList();
}

void SBlueprintVisualDiff::Internal_OnGraphSelectionChanged(TSharedPtr<FGraphToDiff> Item, ESelectInfo::Type SelectionType)
{
	OnGraphSelectionChanged(Item, SelectionType);

	auto GraphOldPtr = NS_BlueprintVisualDiff::StealGraphOld();
	auto GraphOld = Item.Get()->*GraphOldPtr;

	auto GraphNewPtr = NS_BlueprintVisualDiff::StealGraphNew();
	auto GraphNew = Item.Get()->*GraphNewPtr;
	
	UEdGraph* Graph = GraphOld ? GraphOld : GraphNew;

	SelectedGraphPath = FGraphDiffControl::GetGraphPath(Graph);
}

void SBlueprintVisualDiff::Internal_CreateGraphEntry(UEdGraph* GraphOld, UEdGraph* GraphNew)
{
	CreateGraphEntry(GraphOld, GraphNew);
}

SBlueprintVisualDiff::~SBlueprintVisualDiff()
{
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.ExecuteIfBound();
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.Unbind();
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
