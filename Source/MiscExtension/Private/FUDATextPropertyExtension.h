#pragma once

#include "CoreMinimal.h"

class FUDATextPropertyExtension
{
public:
	void Initialize();

	TSharedRef<FExtender> GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects);
};
