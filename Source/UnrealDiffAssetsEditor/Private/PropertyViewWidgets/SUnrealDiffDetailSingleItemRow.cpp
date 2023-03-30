// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"

#include "SlateOptMacros.h"
#include "UnrealDiffWindowStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnrealDiffDetailSingleItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	STableRow< TSharedPtr< FUnrealDiffDetailTreeNode > >::ConstructInternal(
STableRow::FArguments()
	.Style(FUnrealDiffWindowStyle::GetAppStyle(), "DetailsView.TreeView.TableRow")
	.ShowSelection(false),
	InOwnerTableView);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
