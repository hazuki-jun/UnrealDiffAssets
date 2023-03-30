#include "UnrealDiffWindowStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon1920x1080(1920.f, 1080.f);

TSharedPtr< FSlateStyleSet > FUnrealDiffWindowStyle::StyleInstance = nullptr;

void FUnrealDiffWindowStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FUnrealDiffWindowStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const ISlateStyle& FUnrealDiffWindowStyle::Get()
{
	return *StyleInstance;
}

FName FUnrealDiffWindowStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("UnrealDiffWindowStyle"));
	return StyleSetName;
}

const ISlateStyle& FUnrealDiffWindowStyle::GetAppStyle()
{
#if ENGINE_MAJOR_VERSION == 4
	return FEditorStyle::Get();
#else
	return FAppStyle::Get();
#endif
}

FSlateIcon FUnrealDiffWindowStyle::GetAppSlateIcon(const FName StyleName)
{
#if ENGINE_MAJOR_VERSION == 4
	return FSlateIcon(FEditorStyle::GetStyleSetName(), StyleName);
#else
	return FSlateIcon(FAppStyle::GetAppStyleSetName(), StyleName);
#endif
}

const FSlateBrush* FUnrealDiffWindowStyle::GetAppSlateBrush(const FName StyleName)
{
#if ENGINE_MAJOR_VERSION == 4
	return FEditorStyle::Get().GetBrush("Brushes.Title");
#else
	return FAppStyle::Get().GetBrush("Brushes.Title");
#endif
}

const FSlateColor FUnrealDiffWindowStyle::GetAppSlateColor(const FName ColorName)
{
#if ENGINE_MAJOR_VERSION == 4
	return FEditorStyle::Get().GetSlateColor(ColorName);
#else
	return FAppStyle::Get().GetSlateColor(ColorName);
#endif
}

TSharedRef<FSlateStyleSet> FUnrealDiffWindowStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("UnrealDiffWindowStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("UnrealDiffAssets")->GetBaseDir() / TEXT("Resources"));

	Style->Set("UnrealDiffAssets.WindowBackground", new IMAGE_BRUSH(TEXT("Background"), Icon1920x1080));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
