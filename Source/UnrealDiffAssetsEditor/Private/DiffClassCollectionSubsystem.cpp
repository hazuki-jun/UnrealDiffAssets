// Fill out your copyright notice in the Description page of Project Settings.


#include "DiffClassCollectionSubsystem.h"

#include "AssetToolsModule.h"
#include "SupportClassFactory/UnrealDiffSupportClassFactory.h"

UDiffClassCollectionSubsystem::UDiffClassCollectionSubsystem()
{
}

UDiffClassCollectionSubsystem& UDiffClassCollectionSubsystem::Get()
{
	check(GEditor)
	
	return *GEditor->GetEditorSubsystem<UDiffClassCollectionSubsystem>();
}

void UDiffClassCollectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		
		auto UnrealDiffSupportClassFactory = Cast<UUnrealDiffSupportClassFactory>(Class->GetDefaultObject());
		if (UnrealDiffSupportClassFactory)
		{
			SupportedClasses.Add(UnrealDiffSupportClassFactory->GetSupportedClass(), UnrealDiffSupportClassFactory);
		}
	}
}

bool UDiffClassCollectionSubsystem::IsSupported(UObject* Object)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TWeakPtr<IAssetTypeActions> Actions = AssetToolsModule.Get().GetAssetTypeActionsForClass( Object->GetClass() );
	UClass* SupportClass = Actions.Pin()->GetSupportedClass();

	return SupportClass && SupportedClasses.Find(SupportClass->GetFName());
}

TSharedRef<SCompoundWidget> UDiffClassCollectionSubsystem::CreateVisualDiffWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TWeakPtr<IAssetTypeActions> Actions = AssetToolsModule.Get().GetAssetTypeActionsForClass( InLocalAsset->GetClass() );
	UClass* SupportClass = Actions.Pin()->GetSupportedClass();

	if (auto Object = SupportedClasses.Find(SupportClass->GetFName()))
	{
		if (*Object)
		{
			return (*Object)->FactoryCreateVisualWidget(ParentWindow, InLocalAsset, InRemoteAsset);	
		}
	}
	
	return SNew(SWindow);
}
