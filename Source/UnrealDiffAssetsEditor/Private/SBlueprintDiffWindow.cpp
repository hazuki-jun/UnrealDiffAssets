// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlueprintDiffWindow.h"
#include "Styling/SlateStyle.h"
#include "SBlueprintVisualDiff.h"
#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "SBlueprintDiffWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBlueprintDiffWindow::Construct(const FArguments& InArgs)
{
	DiffAssetType = InArgs._DiffAssetType;
	SWindow::Construct(SWindow::FArguments()
		.Title(InArgs._WindowTitel)
		.ClientSize(FVector2D(1000, 800)));

	// auto Brush = FUnrealDiffWindowStyle::Get().GetBrush(TEXT("UnrealDiffAssets.WindowBackground"));
	if (DiffAssetType == EDiffAssetType::None)
	{
		return;
	}
	
	auto DiffWidget = GetBlueprintDiffWidget(InArgs._LocalAsset, InArgs._RemoteAsset);
	SetContent(SNew(SOverlay)
		// + SOverlay::Slot()
		// .HAlign(EHorizontalAlignment::HAlign_Fill)
		// .VAlign(EVerticalAlignment::VAlign_Fill)
		// [
		// 	SNew(SImage)
		// 	.Image(Brush)	
		// ]
	
		+ SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			DiffWidget
		]);

	WindowClosedEvent.AddLambda([](const TSharedRef<SWindow>& InWindow)
	{
		UUnrealDiffAssetDelegate::OnWindowResized.Clear();
		UUnrealDiffAssetDelegate::OnDataTableRowSelected.Unbind();
	});
}

TSharedRef<SCompoundWidget> SBlueprintDiffWindow::GetBlueprintDiffWidget(UObject* LocalAsset, UObject* RemoteAsset)
{
	switch (DiffAssetType)
	{
	case EDiffAssetType::Blueprint:
		return SNew(SBlueprintVisualDiff)
			.ParentWindow(SharedThis(this))
			.LocalAsset(LocalAsset)
			.RemoteAsset(RemoteAsset);
	case EDiffAssetType::DataTable:
		return SNew(SDataTableVisualDiff)
			.ParentWindow(SharedThis(this))
			.LocalAsset(LocalAsset)
			.RemoteAsset(RemoteAsset);
	default:
		return SNew(SBlueprintVisualDiff).ParentWindow(SharedThis(this));
	}
}

TSharedPtr<SBlueprintDiffWindow> SBlueprintDiffWindow::CreateWindow(EDiffAssetType AssetType, UObject* LocalAsset, UObject* RemoteAsset)
{
	if (!LocalAsset || !RemoteAsset)
	{
		return SNew(SBlueprintDiffWindow);
	}

	FText WindowTitle;
	
	switch (AssetType)
	{
	case EDiffAssetType::Blueprint:
		WindowTitle = LOCTEXT("NamelessBlueprintDiff", "Blueprint Diff");
		break;
	case EDiffAssetType::DataTable:
		WindowTitle = LOCTEXT("NamelessDataTableDiff", "DataTable Diff");
		break;
	default:
		return SNew(SBlueprintDiffWindow);
	}
	
	TSharedPtr<SBlueprintDiffWindow> Window =
		SNew(SBlueprintDiffWindow)
			.WindowTitel(WindowTitle)
			.DiffAssetType(AssetType)
			.LocalAsset(LocalAsset)
			.RemoteAsset(RemoteAsset);
					
	
	TSharedPtr<SWindow> ActiveModal = FSlateApplication::Get().GetActiveModalWindow();
	if (ActiveModal.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), ActiveModal.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}

	return Window;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
