#include "FUDATextPropertyExtension.h"

void FUDATextPropertyExtension::Initialize()
{
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	auto OnGetGlobalRowExtensionHandle = EditModule.GetGlobalRowExtensionDelegate().AddStatic(&RegisterAddSourceStringExtensionHandler);
}
