// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintWidgets/SBlueprintVisualDiff.h"

#include "DiffControl.h"
#include "EdGraphUtilities.h"
#include "FileHelpers.h"
#include "GraphDiffControl.h"
#include "K2Node_Composite.h"
#include "K2Node_CreateDelegate.h"
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
		, FSlateIcon(FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintDif.PrevDiff"))
	);
	NavToolBarBuilder.AddToolBarButton(
		FUIAction(
			FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::NextDiff),
			FCanExecuteAction::CreateSP(this, &SBlueprintVisualDiff::HasNextDiff)
		)
		, NAME_None
		, LOCTEXT("NextDiffLabel", "Next")
		, LOCTEXT("NextDiffTooltip", "Go to next difference")
		, FSlateIcon(FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintDif.NextDiff"))
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

	Internal_GenerateDifferencesList();
	
	const auto TextBlock = [](FText Text) -> TSharedRef<SWidget>
	{
		return SNew(SBox)
		.Padding(FMargin(4.0f,10.0f))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Visibility(EVisibility::HitTestInvisible)
			.TextStyle(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.CategoryTextStyle")
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

	const FButtonStyle& ButtonStyle = FUnrealDiffWindowStyle::GetAppStyle().GetWidgetStyle<FButtonStyle>("SimpleButton");
	
	this->ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FUnrealDiffWindowStyle::GetAppStyle().GetBrush( "Docking.Tab", ".ContentAreaBrush" ))
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
						.Padding(0.f)
						.AutoWidth()
						[
							NavToolBarBuilder.MakeWidget()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(FMargin(1.f, 0.f, 0.f, 0.f))
						[
							SNew(SButton)
							.ContentPadding(0.f)
							.Visibility(this, &SBlueprintVisualDiff::GetMergeButtonVisibility)
							.ToolTipText(LOCTEXT("UseSelectedDifferenceFromRemote", "Use remote diffs"))
							.ButtonStyle(&ButtonStyle)
							.OnClicked(this, &SBlueprintVisualDiff::OnMergeClicked)
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Padding(FMargin(0.f, 0.f, 0.f, 0.f))
								[
									SNew(SImage)
									.Image(FUnrealDiffWindowStyle::GetAppSlateBrush("ContentReference.UseSelectionFromContentBrowser"))
								]
								+ SVerticalBox::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Bottom)
								.Padding(FMargin(0.f, 0.f, 0.f, 0.f))
								[
									SNew(STextBlock)
									.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
									.Text(LOCTEXT("UseThisChange", "Merge"))
								]
							]
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
							.BorderImage(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("ToolPanel.GroupBorder"))
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
	const_cast<UBlueprint*>(PanelNew.Blueprint)->OnChanged().AddSP(this, &SBlueprintVisualDiff::Internal_OnBlueprintChanged);
	const_cast<UBlueprint*>(PanelOld.Blueprint)->OnChanged().AddSP(this, &SBlueprintVisualDiff::Internal_OnBlueprintChanged);
}

void SBlueprintVisualDiff::Internal_OnBlueprintChanged(UBlueprint* InBlueprint)
{
	if (InBlueprint == PanelOld.Blueprint || InBlueprint == PanelNew.Blueprint)
	{
		// After a BP has changed significantly, we need to regenerate the UI and set back to initial UI to avoid crashes
		Internal_GenerateDifferencesList();
		SetCurrentMode(NS_BlueprintVisualDiff::MyBlueprintMode);
	}
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
	if (SelectedGraphPath.IsEmpty())
	{
		return;
	}
	
	UEdGraph* GraphOld = nullptr;
	UEdGraph* GraphNew = nullptr;
	TSharedPtr<TArray<FDiffSingleResult>> DiffResults;
	int32 RealDifferencesStartIndex = INDEX_NONE;
	auto GraphOldPtr = NS_BlueprintVisualDiff::StealGraphOld();
	auto GraphNewPtr = NS_BlueprintVisualDiff::StealGraphNew();
	for (const TSharedPtr<FGraphToDiff>& GraphToDiff : Graphs)
	{
		UEdGraph* OldGraph = GraphToDiff.Get()->*GraphOldPtr;
		UEdGraph* NewGraph = GraphToDiff.Get()->*GraphNewPtr;
		
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
}

void SBlueprintVisualDiff::PerformMerge(TSharedPtr<TArray<FDiffSingleResult>> DiffResults, UEdGraph* LocalGraph, UEdGraph* RemoteGraph)
{
	if (!DiffResults || DiffResults->IsEmpty())
	{
		return;
	}
	
	UBlueprint* LocalAssetBlueprint = CastChecked<UBlueprint>(LocalAsset);

	bool bIsFunctionGraph = false;
	if (LocalGraph)
	{
		if (LocalGraph->Nodes.IsValidIndex(0) && LocalGraph->Nodes[0]->GetName().Find(TEXT("K2Node_FunctionEntry")) != INDEX_NONE)
		{
			bIsFunctionGraph = true;
		}
	}
	if (RemoteGraph)
	{
		if (RemoteGraph->Nodes.IsValidIndex(0) && RemoteGraph->Nodes[0]->GetName().Find(TEXT("K2Node_FunctionEntry")) != INDEX_NONE)
		{
			bIsFunctionGraph = true;
		}
	}

	if (bIsFunctionGraph)
	{
		// 对比的是函数
		MergeFunctionGraph(LocalAssetBlueprint, LocalGraph, RemoteGraph);
	}
}

void SBlueprintVisualDiff::MergeFunctionGraph(UBlueprint* Blueprint, UEdGraph* LocalGraph, UEdGraph* RemoteGraph)
{
	if (!LocalGraph && RemoteGraph)
	{
		AddFunctionGraph(Blueprint, RemoteGraph);
	}
	else if (LocalGraph && !RemoteGraph)
	{
		RemoveFunctionGraph(Blueprint, LocalGraph);
	}
	else if (LocalGraph && RemoteGraph)
	{
		RemoveFunctionGraph(Blueprint, LocalGraph);
		
		AddFunctionGraph(Blueprint, RemoteGraph);
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

void SBlueprintVisualDiff::RemoveFunctionGraph(UBlueprint* Blueprint, class UEdGraph* InGraph)
{
	if (InGraph && InGraph->bAllowDeletion)
	{
		if (const UEdGraphSchema* Schema = InGraph->GetSchema())
		{
			if (Schema->TryDeleteGraph(InGraph))
			{
				return;
			}
		}

		const FScopedTransaction Transaction( LOCTEXT("RemoveGraph", "Remove Graph") );
		Blueprint->Modify();

		InGraph->Modify();
		FBlueprintEditorUtils::RemoveGraph(Blueprint, InGraph, EGraphRemoveFlags::Default);

		for (TObjectIterator<UK2Node_CreateDelegate> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFlags */ EInternalObjectFlags::Garbage); It; ++It)
		{
			if (It->GetGraph() != InGraph)
			{
				if (IsValid(*It) && IsValid(It->GetGraph()))
				{
					It->HandleAnyChange();
				}
			}
		}

		InGraph = NULL;
		
	}
	
	FEditorFileUtils::PromptForCheckoutAndSave({ LocalAsset->GetOutermost() }, false, /*bPromptToSave=*/ false);
}

EVisibility SBlueprintVisualDiff::GetMergeButtonVisibility() const
{
	return MergeButtonVisibility;
}

void SBlueprintVisualDiff::Internal_GenerateDifferencesList()
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
		TArray< TSharedPtr<class FBlueprintDifferenceTreeEntry> > TempRealDifferences;
		
		Graph->GenerateTreeEntries(TempTreeEntries, TempRealDifferences);
		
		for (const auto& TreeEntry : TempTreeEntries)
		{
			TreeEntry->OnFocus.Unbind();
			TreeEntry->OnFocus.BindRaw(this, &SBlueprintVisualDiff::Internal_OnGraphSelectionChanged, Graph, ESelectInfo::Direct);
		}

		for (const auto& DiffResultItem : Graph->DiffListSource)
		{
			for (const auto& Differences: TempRealDifferences)
			{
				Differences->OnFocus.Unbind();
				Differences->OnFocus.BindRaw(this, &SBlueprintVisualDiff::Internal_OnDiffListSelectionChanged, DiffResultItem);
			}
		}
		
		PrimaryDifferencesList.Append(MoveTemp(TempTreeEntries));
		RealDifferences.Append(MoveTemp(TempRealDifferences));
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

	MergeButtonVisibility = EVisibility::Visible;
	SelectedGraphPath = FGraphDiffControl::GetGraphPath(Graph);
}

void SBlueprintVisualDiff::Internal_OnDiffListSelectionChanged(TSharedPtr<FDiffResultItem> TheDiff)
{
	MergeButtonVisibility = EVisibility::Collapsed;
	
	SelectedGraphPath = FString();

	OnDiffListSelectionChanged(TheDiff);
}

void SBlueprintVisualDiff::Internal_CreateGraphEntry(UEdGraph* GraphOld, UEdGraph* GraphNew)
{
	CreateGraphEntry(GraphOld, GraphNew);
}

SBlueprintVisualDiff::~SBlueprintVisualDiff()
{
	for (const auto& TreeEntry : PrimaryDifferencesList)
	{
		if (TreeEntry.IsValid())
		{
			TreeEntry->OnFocus.Unbind();
		}
	}

	for (const auto& TreeEntry : RealDifferences)
	{
		if (TreeEntry.IsValid())
		{
			TreeEntry->OnFocus.Unbind();
		}
	}
	
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.ExecuteIfBound();
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.Unbind();
}

FReply SBlueprintVisualDiff::OnMergeClicked()
{
	OnActionMerge();
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
