// Fill out your copyright notice in the Description page of Project Settings.


#include "DiffClassCollectionSubsystem.h"

#include "AssetToolsModule.h"
#include "Dialogs/Dialogs.h"
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

	CollectSupportedClasses();
}

void UDiffClassCollectionSubsystem::CollectSupportedClasses()
{
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		
		auto UnrealDiffSupportClassFactory = Cast<UUnrealDiffSupportClassFactory>(Class->GetDefaultObject());
		if (UnrealDiffSupportClassFactory)
		{
			SupportedClasses.Add(UnrealDiffSupportClassFactory->GetSupportedClass());
		}
	}
}

bool UDiffClassCollectionSubsystem::IsSupported(UObject* Object)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TWeakPtr<IAssetTypeActions> Actions = AssetToolsModule.Get().GetAssetTypeActionsForClass( Object->GetClass() );
	UClass* SupportClass = Actions.Pin()->GetSupportedClass();

	return SupportClass && SupportedClasses.Contains(SupportClass->GetFName());
}

TSharedRef<SWidget> UDiffClassCollectionSubsystem::CreateVisualDiffWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TWeakPtr<IAssetTypeActions> Actions = AssetToolsModule.Get().GetAssetTypeActionsForClass( InLocalAsset->GetClass() );
	UClass* SupportClass = Actions.Pin()->GetSupportedClass();

	if (!SupportedClasses.Contains(SupportClass->GetFName()))
	{
		return SNullWidget::NullWidget;
	}
	
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		
		auto UnrealDiffSupportClassFactory = Cast<UUnrealDiffSupportClassFactory>(Class->GetDefaultObject());
		if (UnrealDiffSupportClassFactory && UnrealDiffSupportClassFactory->GetSupportedClass().IsEqual(SupportClass->GetFName()))
		{
			if (UnrealDiffSupportClassFactory->CanDiff(InLocalAsset, InRemoteAsset))
			{
				return UnrealDiffSupportClassFactory->FactoryCreateVisualWidget(ParentWindow, InLocalAsset, InRemoteAsset);	
			}
			else
			{
				FSuppressableWarningDialog::FSetupInfo Info = UnrealDiffSupportClassFactory->GetDiffFailedDialogInfo();
				if (!Info.Title.IsEmpty())
				{
					FSuppressableWarningDialog RemoveLevelWarning(Info);
					auto Result = RemoveLevelWarning.ShowModal();
				}
			}
		}
	}
	
	return SNullWidget::NullWidget;
}
