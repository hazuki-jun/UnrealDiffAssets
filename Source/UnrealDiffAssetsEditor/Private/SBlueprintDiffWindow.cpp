// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlueprintDiffWindow.h"

#include "DiffClassCollectionSubsystem.h"
#include "Styling/SlateStyle.h"
#include "BlueprintWidgets/SBlueprintVisualDiff.h"
#include "DataTableWidgets/SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "UnrealDiffAssetDelegate.h"
#include "UnrealDiffWindowStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "SBlueprintDiffWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBlueprintDiffWindow::Construct(const FArguments& InArgs)
{
	SWindow::Construct(SWindow::FArguments()
		.Title(InArgs._WindowTitel)
		.ClientSize(FVector2D(1000, 800)));
	
	auto DiffWidget = GetBlueprintDiffWidget(InArgs._LocalAsset, InArgs._RemoteAsset);
	SetContent(SNew(SOverlay)
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
		UUnrealDiffAssetDelegate::OnStringTableRowSelected.Unbind();
	});
}

TSharedRef<SWidget> SBlueprintDiffWindow::GetBlueprintDiffWidget(UObject* LocalAsset, UObject* RemoteAsset)
{
	return GDiffClassCollectionSubsystem.CreateVisualDiffWidget(SharedThis(this), LocalAsset, RemoteAsset);
}

TSharedPtr<SBlueprintDiffWindow> SBlueprintDiffWindow::CreateWindow(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (!LocalAsset || !RemoteAsset)
	{
		return SNew(SBlueprintDiffWindow);
	}
	
	TSharedPtr<SBlueprintDiffWindow> Window =
		SNew(SBlueprintDiffWindow)
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
