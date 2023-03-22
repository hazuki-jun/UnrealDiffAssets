// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlueprintVisualDiff.h"

#include "GraphDiffControl.h"
#include "SlateOptMacros.h"
#include "SMyBlueprint.h"
#include "UnrealDiffAssetDelegate.h"
#include "Kismet/Private/DiffControl.h"
#include "Kismet2/BlueprintEditorUtils.h"

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
		FUIAction(FExecuteAction::CreateSP(this, &SBlueprintVisualDiff::OnActionUseSelected))
		, NAME_None
		, LOCTEXT("UseSelectedLabel", "Merge")
		, LOCTEXT("UseSelectedTooltip", "Use Selected Difference")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.UseSelectionFromContentBrowser")
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

	InArgs._ParentWindow->AddOverlaySlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Top)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(100.f, 48.0f, 0.0f, 0.f))
		[
			GraphToolbarBuilder.MakeWidget()
		]
		
		// .Padding(FMargin(85.f, 45.0f, 0.0f, 0.f))
		// [
		// 	GraphToolbarBuilder.MakeWidget()
		// ]
	];
}

void SBlueprintVisualDiff::OnActionUseSelected()
{
	auto StolenEdGraphPtr = ::StealEdGraph();
	UEdGraph* LocalGraph = PanelOld.MyBlueprint.Get()->*StolenEdGraphPtr;
	UEdGraph* RemoteGraph = PanelNew.MyBlueprint.Get()->*StolenEdGraphPtr;
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
	
	// if (LocalGraph && PanelOld.GraphEditor.IsValid())
	// {
	// 	auto GraphEditorEdGraphPtr = ::StealEdGraphObj();
	// 	UEdGraph* EditorEdGraph = PanelOld.GraphEditor.Pin().Get()->*GraphEditorEdGraphPtr;
	// 	const FString LocalEdGraphPath = FGraphDiffControl::GetGraphPath(LocalGraph);
	// 	const FString EditorEdGraphPath = FGraphDiffControl::GetGraphPath(EditorEdGraph);
	// 	if (!LocalEdGraphPath.Equals(EditorEdGraphPath))
	// 	{
	// 		LocalGraph = nullptr;
	// 	}
	// }
	// else
	// {
	// 	LocalGraph = nullptr;
	// }
	
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
	FString FocusGraphPath;
	if (RemoteGraph)
	{
		FocusGraphPath = FGraphDiffControl::GetGraphPath(RemoteGraph);
	}
	
	if (DiffResults->Num() > 0)
	{
		// 对比的是事件
		
	}
	else
	{
		// 对比的是函数
		
		UBlueprint* LocalAssetBlueprint = CastChecked<UBlueprint>(LocalAsset);
		if (!LocalGraph)
		{
			// 本地蓝图不存在函数 FocusGraphPath
			AddFunctionGraph(LocalAssetBlueprint, RemoteGraph);
		}
		else if (!RemoteGraph)
		{
			// 对比的蓝图不存在 FocusGraphPath
			RemoveFunctionGraph(LocalAssetBlueprint, FocusGraphPath);
		}
	}
}

void SBlueprintVisualDiff::AddFunctionGraph(UBlueprint* Blueprint, UEdGraph* Graph)
{
	FBlueprintEditorUtils::AddFunctionGraph<UClass>(Blueprint, Graph, /*bIsUserCreated=*/ true, nullptr);
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
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
