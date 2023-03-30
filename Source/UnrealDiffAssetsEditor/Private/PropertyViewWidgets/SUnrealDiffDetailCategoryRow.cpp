// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"

#include "SlateOptMacros.h"
#include "PropertyViewWidgets/SUnrealDiffDetailExpanderArrow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailCategoryRow::Construct(const FArguments& InArgs)
{
	TSharedRef<SHorizontalBox> HeaderBox = SNew(SHorizontalBox)
		// + SHorizontalBox::Slot()
		// .HAlign(HAlign_Left)
		// .VAlign(VAlign_Fill)
		// .AutoWidth()
		// [
		// 	SNew(SDetailRowIndent, SharedThis(this))
		// ]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(2, 0, 0, 0)
		.AutoWidth()
		[
			SNew(SUnrealDiffDetailExpanderArrow, SharedThis(this))
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(4, 0, 0, 0)
		.FillWidth(1)
		[
			SNew(STextBlock)
			// .Text(InArgs._DisplayName)
			// .Font(FAppStyle::Get().GetFontStyle(bIsInnerCategory ? PropertyEditorConstants::PropertyFontStyle : PropertyEditorConstants::CategoryFontStyle))
			// .TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
		];
	
	
	ChildSlot
	[
		HeaderBox
	];
	
}

int32 SUnrealDiffDetailCategoryRow::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// TSharedRef< ITypedTableView<ItemType> > OwnerTable = OwnerTablePtr.Pin().ToSharedRef();
	// const bool bIsActive = OwnerTable->AsWidget()->HasKeyboardFocus();
	//
	// if (const ItemType* MyItemPtr = GetItemForThis(OwnerTable))
	// {
	// 	if (bIsActive && OwnerTable->Private_UsesSelectorFocus() && OwnerTable->Private_HasSelectorFocus(*MyItemPtr))
	// 	{
	// 		FSlateDrawElement::MakeBox(
	// 			OutDrawElements,
	// 			LayerId,
	// 			AllottedGeometry.ToPaintGeometry(),
	// 			&Style->SelectorFocusedBrush,
	// 			ESlateDrawEffect::None,
	// 			Style->SelectorFocusedBrush.GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
	// 		);
	// 	}
	// }

	LayerId = SBorder::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (ItemDropZone.IsSet())
	{
		if (PaintDropIndicatorEvent.IsBound())
		{
			LayerId = PaintDropIndicatorEvent.Execute(ItemDropZone.GetValue(), Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		}
		else
		{
			OnPaintDropIndicator(ItemDropZone.GetValue(), Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		}
	}

	return LayerId;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
