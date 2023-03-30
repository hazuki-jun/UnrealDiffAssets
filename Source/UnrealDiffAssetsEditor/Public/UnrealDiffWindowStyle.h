#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FUnrealDiffWindowStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

	static FSlateIcon GetAppSlateIcon(const FName StyleName);
	static const FSlateBrush* GetAppSlateBrush(const FName StyleName);
	
private:
	static TSharedRef< class FSlateStyleSet > Create();
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
