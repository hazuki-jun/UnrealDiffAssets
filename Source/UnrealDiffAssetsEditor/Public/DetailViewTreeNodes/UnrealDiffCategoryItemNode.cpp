#include "UnrealDiffCategoryItemNode.h"

#include "ObjectEditorUtils.h"
#include "UnrealDiffDetailItemNode.h"
#include "PropertyViewWidgets/SUnrealDiffDetailCategoryRow.h"
#include "PropertyViewWidgets/SUnrealDiffDetailView.h"

TSharedRef<ITableRow> FUnrealDiffCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SUnrealDiffDetailCategoryRow, OwnerTable, AsShared()).CategoryName(CategoryName);
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
	for (const auto ChildProperty : ChildPropertyArray)
	{
		TSharedPtr<FUnrealDiffDetailItemNode> DetailItemNode = MakeShareable(new FUnrealDiffDetailItemNode(DetailView));
		{
			DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
			DetailView->AddCacheNode(DetailItemNode);
			DetailItemNode->ParentNode = AsShared();
			DetailItemNode->Property = ChildProperty;
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName());
			if (CastField<FArrayProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else if (CastField<FSetProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else if (CastField<FMapProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else
			{
				DetailItemNode->ValueText = GetValueTextStructInternal(ChildProperty.Get());
			}
			DetailItemNode->DisplayNameText = ChildProperty->GetDisplayNameText();
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
}

void* FUnrealDiffCategoryItemNode::GetStructData(int32 ArrayIndex)
{
	if (!DetailView)
	{
		return nullptr;
	}

	if (auto DataTable = DetailView->GetDataTable())
	{
		for (const auto& RowIt : DataTable->GetRowMap())
		{
			if (RowIt.Key.IsEqual(DetailView->GetCurrentRowName()))
			{
				return RowIt.Value;
			}
		}
	}

	return nullptr;
}

FName FUnrealDiffCategoryItemNode::GetCategoryName()
{
	return CategoryName;
}

FString FUnrealDiffCategoryItemNode::GetParentUniqueNodeId() const
{
	if (CategoryName.IsNone())
	{
		return FString();
	}
	
	return CategoryName.ToString();
}
