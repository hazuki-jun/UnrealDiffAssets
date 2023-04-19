// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UnrealDiffSaveGame.generated.h"

USTRUCT()
struct FUnrealDiffExpandedStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FName> ExpandedCategoryName;
};

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UUnrealDiffSaveGame();
	
	static UUnrealDiffSaveGame* GetSaveGame();
	
	static bool IsRowCategoryExpanded(const FName& RowName, FName CategoryName);

	static void AddExpandedCategory(const FName& RowName, FName CategoryName);

	static void RemoveExpandedCategory(const FName& RowName, FName CategoryName);
	
	static void Save(UUnrealDiffSaveGame* InSaveGame);
	
	UPROPERTY()
	TMap<FName, FUnrealDiffExpandedStruct> DataTableRowExpandedMap;

public:
	static FString PropertyExtension_GetDefaultGlobalStringTable();
	
	static void PropertyExtension_SetDefaultGlobalStringTable(TSoftObjectPtr<class UStringTable> InDefaultGlobalStringTable);
	
	static void PropertyExtension_AddDefaultStringTable(const FName& BlueprintName, const FString& StringTablePath);
	
	static FString PropertyExtension_GetDefaultStringTable(const FName& BlueprintName);
	
	UPROPERTY()
	TMap<FName, FString> PropertyExtension_DefaultStringTables;

	UPROPERTY()
	TMap<FName, FString> PropertyExtension_StringTableKeyPrefix;
	
	UPROPERTY(EditAnywhere, Category = WidgetBlueprint)
	TSoftObjectPtr<class UStringTable> DefaultGlobalStringTable;

	UPROPERTY(EditAnywhere, Category = WidgetBlueprint)
	FString DefaultStringTableKeyPrefix;
};
