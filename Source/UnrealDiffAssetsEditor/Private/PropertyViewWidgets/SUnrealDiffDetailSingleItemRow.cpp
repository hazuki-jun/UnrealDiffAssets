// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"
#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif
#include "HAL/PlatformApplicationMisc.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "DataTableWidgets/SUnrealDiffDataTableRowDetailView.h"
#include "DetailViewTreeNodes/UnrealDiffDetailTreeNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailRowIndent.h"
#include "PropertyViewWidgets/SUnrealDiffPropertyNameWidget.h"
#include "PropertyViewWidgets/SUnrealDiffPropertyValueWidget.h"
#include "Utils/FUnrealDiffDataTableUtil.h"

#define LOCTEXT_NAMESPACE "SUnrealDiffDetailSingleItemRow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailSingleItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FUnrealDiffDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	check(OwnerTreeNode.Pin()->Property.Get());
	
	this->ChildSlot
	[
		BuildRowContent()
	];

	STableRow< TSharedPtr< FUnrealDiffDetailTreeNode > >::ConstructInternal(
		STableRow::FArguments()
			.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.TreeView.TableRow")
			.ShowSelection(false),
			InOwnerTableView);
}

void SUnrealDiffDetailSingleItemRow::Refresh()
{
	auto AttachedWidget = this->ChildSlot.DetachWidget();
	if (AttachedWidget)
	{
		AttachedWidget.Reset();
	}
	this->ChildSlot.AttachWidget(BuildRowContent());
}

TSharedRef<SWidget> SUnrealDiffDetailSingleItemRow::BuildRowContent()
{
	SUnrealDiffDetailView* DetailsView =  OwnerTreeNode.Pin()->GetDetailsView();
	FUnrealDiffDetailColumnSizeData& ColumnSizeData = DetailsView->GetColumnSizeData();
	
	TSharedRef<SWidget> BroderWidget = SNew(SBorder)
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
				SAssignNew(ValueWidget, SUnrealDiffPropertyValueWidget, OwnerTreeNode)
			]
		];


	return SNew( SBorder )
		.BorderImage(this, &SUnrealDiffDetailSingleItemRow::GetBorderImage)
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
							BroderWidget
						]
					]
				]
			]
		];
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
		&& OwnerTreeNode.Pin()->bHasAnyDifference)
		// && !OwnerTreeNode.Pin()->IsContainerNode()
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
	// if (OwnerTreeNode.Pin()->IsContainerNode())
	// {
		FString PropertyString = CopyProperty();
		DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), PropertyString, true);
	// }
	// else
	// {
	// 	DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), OwnerTreeNode.Pin()->ValueText.ToString(), true);
	// }
	
	// if (OwnerTreeNode.Pin()->IsContainerNode())
	// {
	// 	DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), OwnerTreeNode.Pin()->ValueText.ToString(), true);
	// }
	// else
	// {
	// 	DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), OwnerTreeNode.Pin()->ValueText.ToString(), false);
	// 	OwnerTreeNode.Pin()->bHasAnyDifference = false;
	// 	Refresh();
	// }
}

void SUnrealDiffDetailSingleItemRow::OnMenuActionCopy()
{
	FString FormattedString = CopyProperty();
	FPlatformApplicationMisc::ClipboardCopy(*FormattedString);
}

FString SUnrealDiffDetailSingleItemRow::CopyProperty()
{
	return FUnrealDiffDataTableUtil::CopyProperty(OwnerTreeNode.Pin());
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

const FSlateBrush* SUnrealDiffDetailSingleItemRow::GetBorderImage() const
{
#if ENGINE_MAJOR_VERSION == 4
	if( IsHighlighted() )
	{
		return FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Highlighted");
	}
	else if (IsHovered())
	{
		return FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Hovered");
	}
	else
	{
		return FEditorStyle::GetBrush("DetailsView.CategoryMiddle");
	}
#else
	return FUnrealDiffWindowStyle::GetAppStyle().GetBrush("DetailsView.GridLine");
#endif
}

FSlateColor SUnrealDiffDetailSingleItemRow::GetOuterBackgroundColor() const
{
	return FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f));
}

FSlateColor SUnrealDiffDetailSingleItemRow::GetInnerBackgroundColor() const
{
#if ENGINE_MAJOR_VERSION == 4
	return FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f));
#endif
	
	FSlateColor Color;
	
	if (IsHighlighted())
	{
		Color = FUnrealDiffWindowStyle::GetAppStyle().GetSlateColor("Colors.Hover");
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
