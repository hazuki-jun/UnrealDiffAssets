#include "UnrealDiffCategoryItemNode.h"

#include "ObjectEditorUtils.h"
#include "UnrealDiffDetailItemNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

TSharedRef<ITableRow> FUnrealDiffCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailCategoryRow, OwnerTable).CategoryName(CategoryName);
}

void FUnrealDiffCategoryItemNode::GetChildren(TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& OutChildren)
{
	for (auto Child : Children)
	{
		OutChildren.Add(Child);
	}
}

SUnrealDiffDetailView* FUnrealDiffCategoryItemNode::GetDetailsView() const
{
	return DetailView;
}

void FUnrealDiffCategoryItemNode::GenerateChildren()
{
	for (const auto ChildPropertyData : ChildPropertyArray)
	{
		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		DetailItemNode->PropertyData = ChildPropertyData;
		if (const FStructProperty* StructProp = CastField<FStructProperty>(ChildPropertyData->Property.Get()))
		{
			if (DetailView)
			{
				for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
				{
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
