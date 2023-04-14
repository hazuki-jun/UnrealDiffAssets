// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintWidgets/SBlueprintVisualDiff.h"

#include "EdGraphUtilities.h"
#include "GraphDiffControl.h"
#include "K2Node_FunctionEntry.h"
#include "SlateOptMacros.h"
#include "SMyBlueprint.h"

#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffClipboardData.h"
#include "UnrealDiffWindowStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/BlueprintEditorUtils.h"

namespace
{
	static const TCHAR* VAR_PREFIX = TEXT("BPVar");
	static const TCHAR* GRAPH_PREFIX = TEXT("BPGraph");
}

#define STEAL_PROPERTY(Type, Class, Property) \
namespace \
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


STEAL_PROPERTY(UEdGraph*, SMyBlueprint, EdGraph)
STEAL_PROPERTY(UEdGraph*, SGraphEditor, EdGraphObj)

#define LOCTEXT_NAMESPACE "SBlueprintVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBlueprintVisualDiff::Construct(const FArguments& InArgs)
{
	FToolBarBuilder GraphToolbarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	GraphToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::OnActionMerge))
		, NAME_None
		, LOCTEXT("UseSelectedLabel", "Merge")
		, LOCTEXT("UseSelectedTooltip", "Use Selected Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser")
	);

	LocalAsset = InArgs._LocalAsset;
	RemoteAsse = InArgs._RemoteAsset;
	
	UBlueprint* OldBlueprint = CastChecked<UBlueprint>(InArgs._LocalAsset);
	UBlueprint* NewBlueprint = CastChecked<UBlueprint>(InArgs._RemoteAsset);
	
	SBlueprintDiff::Construct(SBlueprintDiff::FArguments()
			.ParentWindow(InArgs._ParentWindow)
 			.BlueprintOld(OldBlueprint)
	 		.BlueprintNew(NewBlueprint)
 			.OldRevision(FRevisionInfo { TEXT("Local") })
 			.NewRevision(FRevisionInfo { TEXT("Remote") })
 			.ShowAssetNames(true));

	FMargin MergeButtonMargin(100.f, 48.0f, 0.0f, 0.f);
#if ENGINE_MAJOR_VERSION == 4
	MergeButtonMargin = FMargin(100.f, 40.0f, 0.0f, 0.f);
#endif
	
	InArgs._ParentWindow->SetTitle(FText::FromString(TEXT("Difference Blueprint")));
	InArgs._ParentWindow->AddOverlaySlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Top)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(MergeButtonMargin)
		// [
		// 	GraphToolbarBuilder.MakeWidget()
		// ]
		
		// .Padding(FMargin(85.f, 45.0f, 0.0f, 0.f))
		// [
		// 	GraphToolbarBuilder.MakeWidget()
		// ]
	];
}

void SBlueprintVisualDiff::OnActionMerge()
{
	UEdGraph* LocalGraph = nullptr;
	UEdGraph* RemoteGraph = nullptr;

	auto StolenEdGraphPtr = ::StealEdGraph();
	LocalGraph = PanelOld.MyBlueprint.Get()->*StolenEdGraphPtr;
	RemoteGraph = PanelNew.MyBlueprint.Get()->*StolenEdGraphPtr;

	if (!LocalGraph && !RemoteGraph)
	{
		return;
	}

	if (LocalGraph)
	{
		const FString LocalEdGraphPath = FGraphDiffControl::GetGraphPath(LocalGraph);
		const FString EditorEdGraphPath = FGraphDiffControl::GetGraphPath(RemoteGraph);
		if (!LocalEdGraphPath.Equals(EditorEdGraphPath))
		{
			LocalGraph = nullptr;
		}
	}
	
	TSharedPtr<TArray<FDiffSingleResult>> DiffResults = MakeShared<TArray<FDiffSingleResult>>();
	FGraphDiffControl::DiffGraphs(LocalGraph, RemoteGraph, *DiffResults);
	struct SortDiff
	{
		bool operator () (const FDiffSingleResult& A, const FDiffSingleResult& B) const
		{
			return A.Diff < B.Diff;
		}
	};
	Sort(DiffResults->GetData(), DiffResults->Num(), SortDiff());
	PerformMerge(DiffResults, LocalGraph, RemoteGraph);
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
		MergeFunctionGraph(LocalAssetBlueprint, LocalGraph, RemoteGraph);
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

SBlueprintVisualDiff::~SBlueprintVisualDiff()
{
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.ExecuteIfBound();
	UUnrealDiffAssetDelegate::OnBlueprintDiffWidgetClosed.Unbind();
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
