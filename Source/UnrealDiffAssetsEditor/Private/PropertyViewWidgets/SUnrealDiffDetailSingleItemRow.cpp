// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailRowIndent.h"
#include "PropertyViewWidgets/SUnrealDiffPropertyNameWidget.h"
#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDetailSingleItemRow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailSingleItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	check(OwnerTreeNode.Pin()->Property.Get());
	
	SUnrealDiffDetailView* DetailsView =  InOwnerTreeNode->GetDetailsView();
	FUnrealDiffDetailColumnSizeData& ColumnSizeData = DetailsView->GetColumnSizeData();
	
	TSharedRef<SWidget> Widget = SNew(SBorder)
		.BorderImage(FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(this, &SUnrealDiffDetailSingleItemRow::GetInnerBackgroundColor)
		.Padding(0)
		[
			SNew(SSplitter)
			.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.Splitter")
			.PhysicalSplitterHandleSize(1.0f)
			.HitDetectionSplitterHandleSize(5.0f)
			+ SSplitter::Slot()
			.Value(ColumnSizeData.GetWholeRowColumnWidth())
			.OnSlotResized(ColumnSizeData.GetOnWholeRowColumnResized())
			[
				SNew(SUnrealDiffPropertyNameWidget, OwnerTreeNode)
			]

			+ SSplitter::Slot()
			.Value(ColumnSizeData.GetWholeRowColumnWidth())
			.OnSlotResized(ColumnSizeData.GetOnWholeRowColumnResized())
			[
				SNew(SUnrealDiffPropertyValueWidget, OwnerTreeNode)
			]
		];
	
	this->ChildSlot
	[
		SNew( SBorder )
		.BorderImage(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("DetailsView.GridLine"))
		.Padding(FMargin(0,0,0,1))
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			SNew(SBox)
			.MinDesiredHeight(22.f)
			[
				SNew( SHorizontalBox )
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew( SBorder )
					.BorderImage(FUnrealDiffWindowStyle::GetAppStyle().GetBrush("DetailsView.CategoryMiddle"))
					.BorderBackgroundColor(this, &SUnrealDiffDetailSingleItemRow::GetOuterBackgroundColor)
					.Padding(0)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SUnrealDiffDetailRowIndent, SharedThis(this))
						]
						
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SBorder)
							.BorderImage(FUnrealDiffWindowStyle::GetAppSlateBrush("DetailsView.CategoryMiddle"))
							.BorderBackgroundColor(this, &SUnrealDiffDetailSingleItemRow::GetInnerBackgroundColor)
							[
								SNew(SBox)
								.WidthOverride(16.f)
								.HeightOverride(16.f)
								[
									SNew(SUnrealDiffDetailExpanderArrow, SharedThis(this))
									.OnExpanderClicked(this, &SUnrealDiffDetailSingleItemRow::OnExpanderClicked)
								]
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							Widget
						]
					]
				]
			]
		]
	];

	STableRow< TSharedPtr< FUnrealDiffDetailTreeNode > >::ConstructInternal(
		STableRow::FArguments()
			.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.TreeView.TableRow")
			.ShowSelection(false),
			InOwnerTableView);
}

FReply SUnrealDiffDetailSingleItemRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
    	TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();
    	FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
    	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
    }
    
    return STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
}

TSharedRef<SWidget> SUnrealDiffDetailSingleItemRow::MakeRowActionsMenu()
{
	if (!OwnerTreeNode.IsValid() || !OwnerTreeNode.Pin()->GetDetailsView())
	{
		return SNullWidget::NullWidget;
	}

	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
				LOCTEXT("DetailSingleItemRowActions_Copy", "Copy"),
				LOCTEXT("DetailSingleItemRowActions_CopyTooltip", "Copy this property value"),
				FUnrealDiffWindowStyle::GetAppSlateIcon("GenericCommands.Copy"),
				FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDetailSingleItemRow::OnMenuActionCopy)));
	
	if (!OwnerTreeNode.Pin()->GetDetailsView()->IsLocalAsset()
		&& !OwnerTreeNode.Pin()->IsContainerNode()
		&& OwnerTreeNode.Pin()->bHasAnyDifference)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DetailSingleItemRowActions_MergeRow", "Use This Change"),
			LOCTEXT("DetailSingleItemRowActions_MergeRowTooltip", "Use This Change"),
			FUnrealDiffWindowStyle::GetAppSlateIcon("ContentReference.UseSelectionFromContentBrowser"),
			FUIAction(FExecuteAction::CreateRaw(this, &SUnrealDiffDetailSingleItemRow::OnMenuActionMerge))
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void SUnrealDiffDetailSingleItemRow::OnMenuActionMerge()
{
	auto DataTableVisual = OwnerTreeNode.Pin()->GetDetailsView()->GetDataTableVisualDiff();
	if (!DataTableVisual)
	{
		return;
	}

	DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), OwnerTreeNode.Pin()->ValueText.ToString());
}

void SUnrealDiffDetailSingleItemRow::OnMenuActionCopy()
{
	const void* ValueAddr = nullptr;
	if (OwnerTreeNode.Pin()->bIsInContainer)
	{
		ValueAddr = OwnerTreeNode.Pin()->RowDataInContainer; 
	}
	else
	{
		ValueAddr = OwnerTreeNode.Pin()->GetStructData();
	}
	
	FString FormattedString;
	OwnerTreeNode.Pin()->Property.Get()->ExportText_Direct(FormattedString, ValueAddr, ValueAddr, nullptr, PPF_Copy);
	FPlatformApplicationMisc::ClipboardCopy(*FormattedString);
}

void SUnrealDiffDetailSingleItemRow::OnExpanderClicked(bool bIsExpanded)
{
	if (!OwnerTreeNode.IsValid())
	{
		return;
	}

	auto DetailView = OwnerTreeNode.Pin()->GetDetailsView();
	if (!DetailView)
	{
		return;
	}

	auto DataTableVisual = DetailView->GetDataTableVisualDiff();
	
	if (!DataTableVisual)
	{
		return;
	}
	
	DataTableVisual->SyncDetailViewAction_Expanded(DetailView->IsLocalAsset(), bIsExpanded, OwnerTreeNode.Pin()->GetNodeIndex());
}

int32 SUnrealDiffDetailSingleItemRow::GetIndentLevelForBackgroundColor()
{
	IndentLevel = GetIndentLevel() - 1;
	return IndentLevel;
}

FSlateColor SUnrealDiffDetailSingleItemRow::GetOuterBackgroundColor() const
{
	return FSlateColor();
}

FSlateColor SUnrealDiffDetailSingleItemRow::GetInnerBackgroundColor() const
{
	FSlateColor Color;

	if (IsHighlighted())
	{
		Color = FAppStyle::Get().GetSlateColor("Colors.Hover");
	}
	else
	{
		int32 ColorIndex = 0;
		static const uint8 ColorOffsets[] =
		{
			0, 4, (4 + 2), (6 + 4), (10 + 6)
		};

		const FSlateColor BaseSlateColor = IsHovered() ? 
			FUnrealDiffWindowStyle::GetAppStyle().GetSlateColor("Colors.Header") : 
			FUnrealDiffWindowStyle::GetAppStyle().GetSlateColor("Colors.Panel");

		const FColor BaseColor = BaseSlateColor.GetSpecifiedColor().ToFColor(true);

		const FColor ColorWithOffset(
			BaseColor.R + ColorOffsets[ColorIndex], 
			BaseColor.G + ColorOffsets[ColorIndex], 
			BaseColor.B + ColorOffsets[ColorIndex]);

		return FSlateColor(FLinearColor::FromSRGBColor(ColorWithOffset));
	}

	return Color;
}

bool SUnrealDiffDetailSingleItemRow::IsHighlighted() const
{
	// TSharedPtr<FUnrealDiffDetailTreeNode> OwnerTreeNodePtr = OwnerTreeNode.Pin();
	// return OwnerTreeNodePtr.IsValid() ? OwnerTreeNodePtr->IsHighlighted() : false;
	return false;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
