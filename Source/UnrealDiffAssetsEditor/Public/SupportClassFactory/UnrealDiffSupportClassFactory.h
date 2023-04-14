// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dialogs/Dialogs.h"
#include "UObject/Object.h"
#include "UnrealDiffSupportClassFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREALDIFFASSETSEDITOR_API UUnrealDiffSupportClassFactory : public UObject
{
	GENERATED_BODY()

public:
	UUnrealDiffSupportClassFactory();

	/**
	 * @brief 支持对比的AssetName
	 * @return 
	 */
	virtual FName GetSupportedClass();

	/**
	 * @brief 是否可以对比
	 * @param InLocalAsset 
	 * @param InRemoteAsset 
	 * @return [default] true
	 */
	virtual bool CanDiff(UObject* InLocalAsset, UObject* InRemoteAsset);

	/**
	 * @brief 对比失败的弹窗信息 -- 不填不弹窗
	 * @return 
	 */
	virtual FSuppressableWarningDialog::FSetupInfo GetDiffFailedDialogInfo();

	/**
	 * @brief 创建对比界面
	 * @param ParentWindow 
	 * @param InLocalAsset 
	 * @param InRemoteAsset 
	 * @return 
	 */
	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset);
	
	FName SupportClassName;
};
