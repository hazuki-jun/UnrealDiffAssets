// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "DiffClassCollectionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UDiffClassCollectionSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	UDiffClassCollectionSubsystem();
	
	static UDiffClassCollectionSubsystem& Get();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CollectSupportedClasses();
	
	bool IsSupported(UObject* Object);

	TSharedRef<SCompoundWidget> CreateVisualDiffWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset);
	
protected:
	TSet<FName> SupportedClasses;
};

#define GDiffClassCollectionSubsystem UDiffClassCollectionSubsystem::Get()
