// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNREALDIFFASSETSEDITOR_API SUnrealDiffDataTableDetailRowSelector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnrealDiffDataTableDetailRowSelector) {}
		SLATE_ARGUMENT(TArray<FName>, Rows)
		SLATE_ARGUMENT(FName, SelectedRowName)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//~ Begin SComboBox 
	virtual void OnSelectionChanged(TSharedPtr<FName> InItem, ESelectInfo::Type InSelectionInfo);
	TSharedRef<SWidget> OnGenerateComboWidget(TSharedPtr<FName> InItem);
	//~ End SComboBox

	FText GetCurrentName() const;

	void OnRowSelected(FName RowName);
	
protected:
	TArray<TSharedPtr<FName>> Rows;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> MyComboBox; 
	FText SelectedRowText;
};
