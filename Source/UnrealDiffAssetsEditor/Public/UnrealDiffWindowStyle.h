#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class UNREALDIFFASSETSEDITOR_API FUnrealDiffWindowStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static const ISlateStyle& Get();
	static FSlateIcon GetMyPluginIcon();
	
	static FName GetStyleSetName();
	static const ISlateStyle& GetAppStyle();
	static FSlateIcon GetAppSlateIcon(const FName StyleName);
	static const FSlateBrush* GetAppSlateBrush(const FName StyleName);
	static FSlateColor GetAppSlateColor(const FName ColorName);
	
private:
	static TSharedRef< class FSlateStyleSet > Create();
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
