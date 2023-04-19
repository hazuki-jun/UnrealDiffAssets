#include "FUDATextPropertyExtension.h"

#include "BlueprintEditorModule.h"
#include "EditorUtilityLibrary.h"
#include "IDetailTreeNode.h"
#include "ObjectEditorUtils.h"
#include "UnrealDiffSaveGame.h"
#include "UnrealDiffWindowStyle.h"
#include "WidgetBlueprint.h"
#include "Async/Async.h"
#include "Blueprint/WidgetTree.h"
#include "Details/SWidgetDetailsView.h"
#include "Interfaces/IMainFrameModule.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Utils/FUnrealDialogueMessage.h"
#include "Utils/FUnrealDiffStringTableUtil.h"

#define STEAL_PROPERTY(Type, Class, Property) \
namespace PropertyExtension\
{ \
	typedef Type Class::* Stolen_##Property##_Ptr; \
	template <Stolen_##Property##_Ptr Property> \
	struct Robber##Property \
	{ \
		friend Stolen_##Property##_Ptr Steal##Property() \
		{ \
			return Property; \
		} \
	}; \
	template struct Robber##Property<&Class::Property>; \
	Stolen_##Property##_Ptr Steal##Property(); \
} \

STEAL_PROPERTY(TWeakPtr<class FWidgetBlueprintEditor>, SWidgetDetailsView, BlueprintEditor)
STEAL_PROPERTY(UWidgetBlueprint*, FWidgetBlueprintEditor, PreviewBlueprint)

#define LOCTEXT_NAMESPACE "FUDATextPropertyExtension"

void FUDATextPropertyExtension::Initialize()
{
	auto& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
	auto& ExtenderDelegates = BlueprintEditorModule.GetMenuExtensibilityManager()->GetExtenderDelegates();
	ExtenderDelegates.Add(FAssetEditorExtender::CreateRaw(this, &FUDATextPropertyExtension::GetBlueprintToolExtender));
	
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	OnGetGlobalRowExtensionHandle = EditModule.GetGlobalRowExtensionDelegate().AddRaw(this, &FUDATextPropertyExtension::RegisterAddSourceStringExtensionHandler);
}

TSharedRef<FExtender> FUDATextPropertyExtension::GetBlueprintToolExtender(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects)
{
	if (ContextSensitiveObjects.Num() <= 0 || !ContextSensitiveObjects[0]->IsA(UWidgetBlueprint::StaticClass()))
	{
		return MakeShared<FExtender>();
	}
	
	auto WidgetBlueprint = Cast<UWidgetBlueprint>(ContextSensitiveObjects[0]);
	if (!WidgetBlueprint)
	{
		return MakeShared<FExtender>();
	}
	
	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	TSharedPtr<class FUICommandList> PluginCommands = MakeShareable(new FUICommandList);
	ToolbarExtender->AddToolBarExtension("Debugging", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FUDATextPropertyExtension::FillToolbar, WidgetBlueprint->GetFName()));
	
	return ToolbarExtender;
}

void FUDATextPropertyExtension::FillToolbar(FToolBarBuilder& ToolbarBuilder, FName BlueprintName)
{
	ToolbarBuilder.BeginSection("MiscExtension");
	{
		ToolbarBuilder.AddComboButton(
			FUIAction(),
			FOnGetContent::CreateRaw(this, &FUDATextPropertyExtension::OnGenerateToolbarMenu, BlueprintName),
			LOCTEXT("MiscExtension_Label", "Misc Extension"),
			FText::GetEmpty(),
			FUnrealDiffWindowStyle::GetMyPluginIcon(),
			false
		);
	}
	ToolbarBuilder.EndSection();
}

TSharedRef<SWidget> FUDATextPropertyExtension::OnGenerateToolbarMenu(FName BlueprintName)
{
	FMenuBuilder MenuBuilder(true, NULL);
	// MenuBuilder.AddMenuEntry(
	// 			LOCTEXT("PropertyExtensionAddSourceString_Label", "Add Source String"),
	// 			LOCTEXT("PropertyExtensionAddSourceString_Tooltip", "Add a source string to string table"),
	// 			FUnrealDiffWindowStyle::GetAppSlateIcon("Plus"),
	// 			FUIAction(FExecuteAction::CreateRaw(this, &FUDATextPropertyExtension::OnExtension_AddSourceString, BlueprintName)));

	MenuBuilder.AddMenuEntry(
			LOCTEXT("PropertyExtensionSetStringTable_Label", "Set String Table"),
			LOCTEXT("PropertyExtensionSetStringTable_Tooltip", "Set a string table for this widget blueprint"),
			FUnrealDiffWindowStyle::GetIcon("UnrealDiffAssets.ViewOptions"),
			FUIAction(FExecuteAction::CreateRaw(this, &FUDATextPropertyExtension::OnExtension_SetStringTable, BlueprintName)));
	
	return MenuBuilder.MakeWidget();
}

void FUDATextPropertyExtension::OnExtension_AddSourceString(FName BlueprintName)
{
	
}

void FUDATextPropertyExtension::OnExtension_SetStringTable(FName BlueprintName)
{
	CreateSettingsWindow(BlueprintName);
}

void FUDATextPropertyExtension::CreateSettingsWindow(FName BlueprintName)
{
	MyStringTableText = FText::FromString(UUnrealDiffSaveGame::PropertyExtension_GetStringTable(BlueprintName)); 
	
	FVector2D MousePos = FSlateApplication::Get().GetCursorPos();
	TSharedPtr<SWindow> Window =
		SNew(SWindow)
		.Title(FText::FromString(TEXT("Set String Table")))
		.ClientSize(FVector2D(200.f,60.f))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
	;

	Window->MoveWindowTo(MousePos);
	
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	if (MainFrameModule.GetParentWindow().IsValid())
	{
		// FSlateApplication::Get().AddModalWindow(Window.ToSharedRef(), MainFrameModule.GetParentWindow().ToSharedRef());
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), MainFrameModule.GetParentWindow().ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}

	Window->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(80.f)
				.HeightOverride(20.f)
				[
					SNew(SEditableTextBox)
					.Text(this, &FUDATextPropertyExtension::OnGetMyStringTableText)
					.IsReadOnly(true)
				]
			]
			
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(5.f, 0.f, 2.f, 0.f))
			[
				SNew(SButton)
				.OnClicked(this, &FUDATextPropertyExtension::OnUseAssetButtonClicked, BlueprintName)
				[
					SNew(SImage)
	#if ENGINE_MAJOR_VERSION == 5
					.Image(FUnrealDiffWindowStyle::GetAppSlateBrush("Icons.Use"))
	#else
					.Image(FUnrealDiffWindowStyle::GetAppSlateBrush("PropertyWindow.Button_Use"))
	#endif
				]
			]
		]
		
		// Prefix Setting
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(2.f, 3.f, 2.f, 0.f))
			[
				SNew(STextBlock)
				.Text(LOCTEXT("MiscExtension", "Prefix:"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(FMargin(3.f, 3.f, 1.f, 0.f))
			[
				SNew(SEditableTextBox)
				.Justification(ETextJustify::Center)
				.Text(FText::FromString(UUnrealDiffSaveGame::PropertyExtension_GetBlueprintStringTableKeyPrefix(BlueprintName)))
				.OnTextCommitted_Lambda([this, BlueprintName](const FText& InText, ETextCommit::Type InTextCommit)
				{
					if (!InText.IsEmptyOrWhitespace())
					{
						UUnrealDiffSaveGame::PropertyExtension_AddBlueprintStringTableKeyPrefix(BlueprintName, InText.ToString());
					}
				})
			]
		]
	)
	;
}

FReply FUDATextPropertyExtension::OnUseAssetButtonClicked(FName InBlueprintName)
{
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() <= 0 || !SelectedAssets[0]->IsA(UStringTable::StaticClass()))
	{
		return FReply::Unhandled();	
	}

	UStringTable* StringTable = Cast<UStringTable>(SelectedAssets[0]);
	MyStringTableText = FText::FromString(SelectedAssets[0]->GetFName().ToString());
	UUnrealDiffSaveGame::PropertyExtension_AddStringTable(InBlueprintName, StringTable->GetStringTableId().ToString());
	
	return FReply::Unhandled();
}

#if ENGINE_MAJOR_VERSION == 4
void FUDATextPropertyExtension::RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& InArgs, FOnGenerateGlobalRowExtensionArgs::EWidgetPosition InWidgetPosition, TArray<TSharedRef<SWidget>>& OutExtensions)
{
	if (InWidgetPosition != FOnGenerateGlobalRowExtensionArgs::EWidgetPosition::Right)
	{
		return;
	}
	
	if (!InArgs.PropertyHandle)
	{
		return;
	}

	if (!CastField<FTextProperty>(InArgs.PropertyHandle->GetProperty()))
	{
		return;
	}

	CachedPropertyHandle.AddUnique(InArgs.PropertyHandle);
	
	TSharedRef<SWidget> OutWidget =
		SNew(SButton)
		.OnClicked(this, &FUDATextPropertyExtension::ApplySourceString_UE4, CachedPropertyHandle[CachedPropertyHandle.Num() - 1])
		.ToolTipText(NSLOCTEXT("MiscExtension", "AddSourceStringToolTip", "Apply this source string to string table"))
		[
			SNew(SImage)
			.Image(FUnrealDiffWindowStyle::Get().GetBrush("UnrealDiffAssets.Apply"))
		];
	
	OutExtensions.Add(OutWidget);
}

FReply FUDATextPropertyExtension::ApplySourceString_UE4(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	ApplySourceString(PropertyHandle);
	return FReply::Unhandled();
}
#endif

#if ENGINE_MAJOR_VERSION == 5
void FUDATextPropertyExtension::RegisterAddSourceStringExtensionHandler(const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>& OutExtensionButtons)
{
	if (!Args.PropertyHandle)
	{
		return;
	}

	if (!CastField<FTextProperty>(Args.PropertyHandle->GetProperty()))
	{
		return;
	}
	
	CachedPropertyHandle.AddUnique(Args.PropertyHandle);
	static FSlateIcon ApplyIcon(FUnrealDiffWindowStyle::GetIcon("UnrealDiffAssets.Apply"));
	FPropertyRowExtensionButton& ApplyButton = OutExtensionButtons.AddDefaulted_GetRef();
	ApplyButton.Icon = ApplyIcon;
	ApplyButton.Label = NSLOCTEXT("MiscExtension", "AddSourceString", "Aplply Source String");
	ApplyButton.ToolTip = NSLOCTEXT("MiscExtension", "AddSourceStringToolTip", "Apply this source string to string table");
	ApplyButton.UIAction = FUIAction(FExecuteAction::CreateRaw(this, &FUDATextPropertyExtension::ApplySourceString, CachedPropertyHandle[CachedPropertyHandle.Num() - 1]));
}
#endif

void FUDATextPropertyExtension::ApplySourceString(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	auto BlueprintName = GetActiveWidgetBlueprintName();
	
	FString StringTablePath = UUnrealDiffSaveGame::PropertyExtension_GetStringTable(BlueprintName);
	if (StringTablePath.IsEmpty())
	{
		// 此蓝图的StringTable未设置

		// 用全局设置
		FString DefaultGlobalStringTable = UUnrealDiffSaveGame::PropertyExtension_GetDefaultGlobalStringTable();
		if (DefaultGlobalStringTable.IsEmpty())
		{
			FUnrealDialogueMessage::ShowModal(FText::FromString(TEXT("StringTable 未设置")), FText::FromString(TEXT("警告")));
			return;
		}
		else
		{
			StringTablePath = DefaultGlobalStringTable;
		}	
	}
	
	if (!PropertyHandle.IsValid())
	{
		return;
	}

	FTextProperty* TextProperty = CastField<FTextProperty>(PropertyHandle->GetProperty());
	if (!TextProperty)
	{
		return;
	}
	
	FText Value;
	PropertyHandle->GetValue(Value);
	if (Value.IsEmpty())
	{
		return;
	}

	auto StringTable = GetStringTable(StringTablePath);
	FString Key = IncrementStringTableSourceString(StringTable, BlueprintName, Value.ToString());
	if (FUnrealDiffStringTableUtil::AddRow(StringTable, Key, Value.ToString()))
	{
		auto Editor = GetActiveWidgetBlueprintEditor();
		if (!Editor.IsValid())
		{
			return;
		}
		
		FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "PasteProperty", "Paste Property"));
		FString FormattedString = FString::Format(TEXT("LOCTABLE(\"{0}\", \"{1}\")"), {StringTablePath, Key});
#if ENGINE_MAJOR_VERSION == 5
		PropertyHandle->SetValueFromFormattedString(FormattedString, EPropertyValueSetFlags::InstanceObjects);
#else
		PropertyHandle->SetValueFromFormattedString(FormattedString, EPropertyValueSetFlags::DefaultFlags);
#endif
		const auto &SelectedWidgets = Editor.Pin()->GetSelectedWidgets();
		for (const FWidgetReference& WidgetRef : SelectedWidgets)
		{
			UWidget* PreviewWidget = WidgetRef.GetPreview();
			if (PreviewWidget)
			{
				FName PreviewWidgetName = PreviewWidget->GetFName();
				auto Blueprint = GetActiveWidgetBlueprint();
				UWidget* TemplateWidget = Blueprint->WidgetTree->FindWidget(PreviewWidgetName);
				FObjectEditorUtils::MigratePropertyValue(PreviewWidget, TextProperty, TemplateWidget, TextProperty);
				FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
			}
		}
	}
}

FString FUDATextPropertyExtension::IncrementStringTableSourceString(const UStringTable* InStringTable, const FName& InBlueprintName, const FString& InSourceString)
{
	if (!InStringTable)
	{
		return FString(); 
	}

	auto StringTableRef = InStringTable->GetStringTable();

	TArray<FString> Keys;

	FString Prefix = UUnrealDiffSaveGame::PropertyExtension_GetBlueprintStringTableKeyPrefix(InBlueprintName);
	if (Prefix.IsEmpty())
	{
		return Prefix;
	}
	
	FString FoundKey;
	StringTableRef->EnumerateSourceStrings([&](const FString& InKey, const FString& SourceString) -> bool
	{
		if (SourceString.Equals(InSourceString))
		{
			FoundKey = InKey;
		}
		Keys.Add(InKey);
		return true;
	});

	if (!FoundKey.IsEmpty())
	{
		return FoundKey; 
	}
	
	return Prefix + FString(TEXT("_")) + FString::FromInt(Keys.Num());
}

FName FUDATextPropertyExtension::GetActiveWidgetBlueprintName()
{
	auto Preview  = GetActiveWidgetBlueprint();
	if (Preview)
	{
		return Preview->GetFName();
	}

	return NAME_None;
}

UWidgetBlueprint* FUDATextPropertyExtension::GetActiveWidgetBlueprint()
{
	const auto WidgetPrintEditor = GetActiveWidgetBlueprintEditor();
	if (WidgetPrintEditor.IsValid())
	{
		auto StolenPreviewBlueprint =  PropertyExtension::StealPreviewBlueprint();
		auto Preview  = WidgetPrintEditor.Pin().Get()->*StolenPreviewBlueprint;
		return Preview;
	}

	return nullptr;
}

TWeakPtr<FWidgetBlueprintEditor> FUDATextPropertyExtension::GetActiveWidgetBlueprintEditor()
{
	TSharedPtr<class SDockTab> Tab = FGlobalTabmanager::Get()->GetActiveTab();
	auto Conent = Tab->GetContent();
	auto WidgetDetailsView = StaticCastSharedRef<SWidgetDetailsView>(Conent);
	auto BlueprintEditorPtr = PropertyExtension::StealBlueprintEditor();
	auto BlueprintEditor=  WidgetDetailsView.Get().*BlueprintEditorPtr;

	return BlueprintEditor;
}

FText FUDATextPropertyExtension::OnGetMyStringTableText() const
{
	return MyStringTableText;
}

UStringTable* FUDATextPropertyExtension::GetStringTable(const FString& InStringTableName)
{
	const auto OutStringTable = Cast<UStringTable>(StaticLoadObject(UStringTable::StaticClass(), nullptr, *InStringTableName));
	return OutStringTable;
}


#undef LOCTEXT_NAMESPACE
