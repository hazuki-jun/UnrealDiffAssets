// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscExtension.h"

#include "BlueprintEditorModule.h"
#include "FUDATextPropertyExtension.h"
#include "UMGEditorModule.h"
#include "Blueprint/WidgetTree.h"


#define LOCTEXT_NAMESPACE "FMiscExtensionModule"

void FMiscExtensionModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FMiscExtensionModule::OnEngineInit);
}

void FMiscExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FMiscExtensionModule::OnEngineInit()
{
	TextPropertyExtensionPtr = MakeShareable(new FUDATextPropertyExtension);
	TextPropertyExtensionPtr->Initialize();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscExtensionModule, MiscExtension)
