﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealDiffSaveGame.h"

#include "Kismet/GameplayStatics.h"

namespace 
{
	const FString SlotName = TEXT("UnrealDiff");
	constexpr int32 SlotIndex = 0;
}

UUnrealDiffSaveGame::UUnrealDiffSaveGame()
{
	
}

UUnrealDiffSaveGame* UUnrealDiffSaveGame::GetSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		return Cast<UUnrealDiffSaveGame>(UGameplayStatics::CreateSaveGameObject(UUnrealDiffSaveGame::StaticClass()));
	}
	
	return Cast<UUnrealDiffSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
}

bool UUnrealDiffSaveGame::IsRowCategoryExpanded(const FName& RowName, FName CategoryName)
{
	if (CategoryName.IsNone())
	{
		return false;
	}
	
	auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return false;	
	}

	if (const auto Found = SaveGame->DataTableRowExpandedMap.Find(RowName))
	{
		return Found->ExpandedCategoryName.Contains(CategoryName);
	}

	return false;
}


void UUnrealDiffSaveGame::AddExpandedCategory(const FName& RowName, FName CategoryName)
{
	if (CategoryName.IsNone())
	{
		return;
	}
	
	auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;	
	}
	
	auto& Found = SaveGame->DataTableRowExpandedMap.FindOrAdd(RowName);
	Found.ExpandedCategoryName.Emplace(CategoryName);

	Save(SaveGame);
}

void UUnrealDiffSaveGame::RemoveExpandedCategory(const FName& RowName, FName CategoryName)
{
	if (CategoryName.IsNone())
	{
		return;
	}
	
	auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;	
	}
	
	if (const auto Found = SaveGame->DataTableRowExpandedMap.Find(RowName))
	{
		Found->ExpandedCategoryName.Remove(CategoryName);
		Save(SaveGame);
	}
}

void UUnrealDiffSaveGame::Save(UUnrealDiffSaveGame* InSaveGame)
{
	UGameplayStatics::SaveGameToSlot(InSaveGame, SlotName, SlotIndex);
}

void UUnrealDiffSaveGame::PropertyExtension_AddBlueprintStringTableKeyPrefix(const FName& InBlueprintName, const FString& InPrefix)
{
	if (InBlueprintName.IsNone() || InPrefix.IsEmpty())
	{
		return;
	}
	
	auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;	
	}

	SaveGame->PropertyExtension_StringTableKeyPrefix.Emplace(InBlueprintName, InPrefix);
	Save(SaveGame);
}

FString UUnrealDiffSaveGame::PropertyExtension_GetBlueprintStringTableKeyPrefix(const FName& InBlueprintName)
{
	if (InBlueprintName.IsNone())
	{
		return FString();
	}

	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return FString();
	}

	if (const auto Prefix = SaveGame->PropertyExtension_StringTableKeyPrefix.Find(InBlueprintName))
	{
		return *Prefix;
	}
	else
	{
		return SaveGame->DefaultGlobalStringTableKeyPrefix;
	}
}

void UUnrealDiffSaveGame::PropertyExtension_SetDefaultGlobalStringTableKeyPrefix(const FString& InPrefix)
{
	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;
	}

	SaveGame->DefaultGlobalStringTableKeyPrefix = InPrefix;
	Save(SaveGame);
}

FString UUnrealDiffSaveGame::PropertyExtension_GetDefaultGlobalStringTable()
{
	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return FString();
	}

	if (!SaveGame->DefaultGlobalStringTable.IsNull())
	{
		return SaveGame->DefaultGlobalStringTable.ToSoftObjectPath().ToString();
	}

	return FString();
}

void UUnrealDiffSaveGame::PropertyExtension_SetDefaultGlobalStringTable(TSoftObjectPtr<UStringTable> InDefaultGlobalStringTable)
{
	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;
	}

	SaveGame->DefaultGlobalStringTable = InDefaultGlobalStringTable;
	Save(SaveGame);
}

void UUnrealDiffSaveGame::PropertyExtension_AddStringTable(const FName& BlueprintName, const FString& StringTablePath)
{
	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return;
	}


	if (BlueprintName.IsNone() || StringTablePath.IsEmpty())
	{
		return;
	}
	
	SaveGame->PropertyExtension_DefaultStringTables.Emplace(BlueprintName, StringTablePath);
	Save(SaveGame);
}

FString UUnrealDiffSaveGame::PropertyExtension_GetStringTable(const FName& BlueprintName)
{
	const auto SaveGame = GetSaveGame();
	if (!SaveGame)
	{
		return FString();
	}

	if (BlueprintName.IsNone())
	{
		return FString();
	}

	SaveGame->PropertyExtension_DefaultStringTables.Remove(NAME_None);
	
	if (const auto Found = SaveGame->PropertyExtension_DefaultStringTables.Find(BlueprintName))
	{
		return *Found;
	}

	return FString();
}
