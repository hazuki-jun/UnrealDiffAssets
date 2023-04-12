// Fill out your copyright notice in the Description page of Project Settings.


#include "SVisualDiffWidget.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "Widgets/Images/SLayeredImage.h"

#define LOCTEXT_NAMESPACE "SVisualDiffWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SVisualDiffWidget::Construct(const FArguments& InArgs)
{
}

TSharedRef<SWidget> SVisualDiffWidget::MakeToolbar()
{
	TSharedRef<SOverlay> Overlay = SNew(SOverlay);
	
	FToolBarBuilder DataTableVisualToolbarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	DataTableVisualToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_HighlightPrevDifference))
		, NAME_None
		, LOCTEXT("PrevLabel", "Prev")
		, LOCTEXT("PrevTooltip", "Prev Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintMerge.PrevDiff")
	);

	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_HighlightNextDifference))
		, NAME_None
		, LOCTEXT("Nextabel", "Next")
		, LOCTEXT("NextTooltip", "Next Difference")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("BlueprintMerge.NextDiff")
	);

	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_Diff))
		, NAME_None
		, LOCTEXT("DiffLabel", "Diff")
		, LOCTEXT("DiffTooltip", "Diff")
		, FUnrealDiffWindowStyle::GetAppSlateIcon("SourceControl.Actions.Diff")
	);
	
	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_Merge))
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
		.Image(FUnrealDiffWindowStyle::Get().GetBrush("UnrealDiffAssets.ViewOptions"))
#else
		.Image(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("DetailsView.ViewOptions"))
#endif
		.ColorAndOpacity(FSlateColor::UseForeground());

	TSharedRef<SComboButton> ComboButton =
		SNew(SComboButton)
		.HasDownArrow(false)
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
#if ENGINE_NET_VERSION == 4
		.ButtonStyle(FUnrealDiffWindowStyle::GetAppStyle(), "NoBorder")
#else
		.ButtonStyle(FUnrealDiffWindowStyle::GetAppStyle(), "SimpleButton")
#endif
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

void SVisualDiffWidget::ToolbarAction_HighlightNextDifference()
{
}

void SVisualDiffWidget::ToolbarAction_HighlightPrevDifference()
{
}

void SVisualDiffWidget::ToolbarAction_Diff()
{
}

bool SVisualDiffWidget::ToolbarAction_CanDiff()
{
	return false;
}

void SVisualDiffWidget::ToolbarAction_Merge()
{
}

TSharedRef<SWidget> SVisualDiffWidget::GetShowViewOptionContent()
{
	FMenuBuilder DetailViewOptions( true, nullptr);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyNormal", "Show Only Normal Rows"),
	LOCTEXT("ShowOnlyNormal_ToolTip", "Displays only row"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EUnrealVisualDiff::Normal),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EUnrealVisualDiff::Normal )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyModified", "Show Only Modified Rows"),
	LOCTEXT("ShowOnlyModified_ToolTip", "Displays only row which have been changed"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EUnrealVisualDiff::Modify),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EUnrealVisualDiff::Modify )
			),
		NAME_None,
		EUserInterfaceActionType::Check);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyAdded", "Show Only Added Rows"),
	LOCTEXT("ShowOnlyAdded_ToolTip", "Displays only row which added"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EUnrealVisualDiff::Added),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EUnrealVisualDiff::Added )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	return DetailViewOptions.MakeWidget();
}

void SVisualDiffWidget::OnShowOnlyViewOptionClicked(EUnrealVisualDiff::RowViewOption InViewOption)
{
	if (InViewOption == EUnrealVisualDiff::Added)
	{
		ReverseRowViewOption(EUnrealVisualDiff::Added);
		ReverseRowViewOption(EUnrealVisualDiff::Removed);
	}
	else
	{
		ReverseRowViewOption(InViewOption);
	}
	
	RefreshLayout();
	ModifyConfig();
}

void SVisualDiffWidget::RefreshLayout()
{
	
}

bool SVisualDiffWidget::HasRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption) const
{
	return (RowViewOption & InViewOption) != 0;
}

void SVisualDiffWidget::ClearRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = RowViewOption & ~InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SVisualDiffWidget::SetRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = RowViewOption | InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SVisualDiffWidget::SetRowViewOptionTo(EUnrealVisualDiff::RowViewOption InViewOption)
{
	RowViewOption = InViewOption;
}

void SVisualDiffWidget::ReverseRowViewOption(EUnrealVisualDiff::RowViewOption InViewOption)
{
	HasRowViewOption(InViewOption) ? ClearRowViewOption(InViewOption) : SetRowViewOption(InViewOption);
}

void SVisualDiffWidget::ModifyConfig()
{

}

bool SVisualDiffWidget::IsShowOnlyRowViewOptionChecked(EUnrealVisualDiff::RowViewOption InViewOption) const
{
	return HasRowViewOption(InViewOption);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
