// Fill out your copyright notice in the Description page of Project Settings.


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
