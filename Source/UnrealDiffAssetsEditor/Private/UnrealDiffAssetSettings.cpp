// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealDiffAssetSettings.h"

#include "UnrealDiffSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "Internationalization/StringTable.h"

#define LOCTEXT_NAMESPACE "UUnrealDiffAssetSettings"

UUnrealDiffAssetSettings::UUnrealDiffAssetSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName  = TEXT("UnrealDiffAsset");
}

#if WITH_EDITOR
FText UUnrealDiffAssetSettings::GetSectionText() const
{
	return LOCTEXT("SettingsDisplayName", "UnrealDiffAsset");
}

void UUnrealDiffAssetSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName().IsEqual(TEXT("DefaultGlobalStringTable")))
	{
		UUnrealDiffSaveGame::PropertyExtension_SetDefaultGlobalStringTable(DefaultGlobalStringTable);
		return;
	}
	else if (PropertyChangedEvent.Property->GetFName().IsEqual(TEXT("DefaultStringTableKeyPrefix")))
	{
		UUnrealDiffSaveGame::PropertyExtension_SetDefaultGlobalStringTable(DefaultGlobalStringTable);
	}
	
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultUnrealDiffAssetSettings.ini"));
	GConfig->Flush(true, FileName);
}
#endif


#undef LOCTEXT_NAMESPACE
