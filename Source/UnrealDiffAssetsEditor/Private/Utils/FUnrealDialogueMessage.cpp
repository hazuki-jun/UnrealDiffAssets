#include "Utils/FUnrealDialogueMessage.h"

#include "Dialogs/Dialogs.h"

void FUnrealDialogueMessage::ShowModal(const FText& Message, const FText& Title)
{
	FSuppressableWarningDialog::FSetupInfo Info(Message, Title, FString());
	Info.ConfirmText = FText::FromString("OK");
	Info.CancelText = FText::FromString("Cancel");
    FSuppressableWarningDialog Dialog(Info);
    auto Result = Dialog.ShowModal();
}
