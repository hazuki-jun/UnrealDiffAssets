#pragma once

#include "CoreMinimal.h"

class UNREALDIFFASSETSEDITOR_API FUnrealDialogueMessage
{
public:
	static void ShowModal(const FText& Message, const FText& Title);
};
