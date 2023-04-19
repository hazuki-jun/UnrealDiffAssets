// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UnrealDiffAssetSettings.generated.h"

/**
 * 
 */
UCLASS(config = UnrealDiffAssetSettings, defaultconfig)
class UNREALDIFFASSETSEDITOR_API UUnrealDiffAssetSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UUnrealDiffAssetSettings();

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//~ Begin DataTableDiff Config
	UPROPERTY(EditAnywhere, Config, Category = DataTableDiff, DisplayName = "ShowOnlyNormal")
	bool DataTableVisualDiffShowOnlyNormal = true;

	UPROPERTY(EditAnywhere, Config, Category = DataTableDiff, DisplayName = "ShowOnlyModify")
	bool DataTableVisualDiffShowOnlyModify = true;

	UPROPERTY(EditAnywhere, Config, Category = DataTableDiff, DisplayName = "ShowOnlyAdded")
	bool DataTableVisualDiffShowOnlyAdded = true;

	// UPROPERTY(EditAnywhere, Config, Category = DataTableDiff, DisplayName = "ShowOnlyRemoved")
	// bool DataTableVisualDiffShowOnlyRemoved = true;
	//~ End DataTableDiff Config
	
	//~ Begin StringTableDiff Config
	UPROPERTY(EditAnywhere, Config, Category = StringTableDiff, DisplayName = "StringTableShowOnlyNormal")
	bool StringTableVisualDiffShowOnlyNormal = true;

	UPROPERTY(EditAnywhere, Config, Category = StringTableDiff, DisplayName = "StringTableShowOnlyModify")
	bool StringTableVisualDiffShowOnlyModify = true;

	UPROPERTY(EditAnywhere, Config, Category = StringTableDiff, DisplayName = "StringTableShowOnlyAdded")
	bool StringTableVisualDiffShowOnlyAdded = true;
	//~ End StringTableDiff Config

	/** 默认 StringTable 界面蓝图未设置时会使用此张表 */
	UPROPERTY(EditAnywhere, Config, Category = WidgetBlueprint)
	TSoftObjectPtr<class UStringTable> DefaultGlobalStringTable;

	/** 默认 StringTable的Key 前缀 界面蓝图未设置时会使用前缀 */
	UPROPERTY(EditAnywhere, Config, Category = WidgetBlueprint)
	FString DefaultGlobalStringTableKeyPrefix = TEXT("DefaultKey");
};
