#include "UnrealDiffDetailItemNode.h"

#include "ObjectEditorUtils.h"
#include "PropertyViewWidgets/SUnrealDiffDetailSingleItemRow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

TSharedRef<ITableRow> FUnrealDiffDetailItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailSingleItemRow, OwnerTable, AsShared());
}

void FUnrealDiffDetailItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	for (auto Child : Children)
	{
		OutChildren.Add(Child);
	}
}

SUnrealDiffDetailView* FUnrealDiffDetailItemNode::GetDetailsView() const
{
	return DetailView;
}

void FUnrealDiffDetailItemNode::GenerateChildren()
{
	for (const auto ChildProperty : ChildPropertyArray)
	{
		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		DetailItemNode->PropertyData = ChildProperty;
		
		if (const FStructProperty* StructProp = CastField<FStructProperty>(ChildProperty->Property.Get()))
		{
			if (DetailView)
			{
				for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
				{
					// auto StructData = StructOnScope->GetStructMemory();
					// TempPropertyData->StructData = It->ContainerPtrToValuePtr<uint8>(StructData, 0);
					
					TSharedPtr<FStructOnScope> StructOnScope = MakeShareable(new FStructOnScope(StructProp->Struct));
					TSharedPtr<FUnrealDiffPropertyData> TempPropertyData = MakeShareable(new FUnrealDiffPropertyData());
					TempPropertyData->ParentStructProperty = StructProp;
					TempPropertyData->Property = *It;
					TempPropertyData->StructOnScope = StructOnScope;
					DetailItemNode->ChildPropertyArray.Add(TempPropertyData);
				}
			}		
		}
		DetailItemNode->GenerateChildren();
		Children.Add(DetailItemNode);
	}
}

