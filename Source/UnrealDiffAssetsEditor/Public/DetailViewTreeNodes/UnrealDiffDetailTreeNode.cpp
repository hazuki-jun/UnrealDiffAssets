#include "UnrealDiffDetailTreeNode.h"

FUnrealDiffDetailTreeNode::FUnrealDiffDetailTreeNode()
{
}

FUnrealDiffDetailTreeNode::~FUnrealDiffDetailTreeNode()
{
}

bool FUnrealDiffDetailTreeNode::IsMapElement() const
{
	return bIsMapElement;
}

bool FUnrealDiffDetailTreeNode::IsContainerNode() const
{
	if (CastField<FStructProperty>(Property.Get()))
	{
		return true;
	}
	
	if (CastField<FMapProperty>(Property.Get()))
	{
		return true;
	}

	if (CastField<FArrayProperty>(Property.Get()))
	{
		return true;
	}

	if (CastField<FSetProperty>(Property.Get()))
	{
		return true;
	}
	
	return Children.Num() > 0;
}

bool FUnrealDiffDetailTreeNode::IsMapKeyOrValue()
{
	if (Property.Get())
	{
		return CastField<FMapProperty>(Property.Get()) && ContainerIndex >= 0;
	}

	return false;
}

FString FUnrealDiffDetailTreeNode::GetPropertyValueAsString()
{
	return FString();
}

FText FUnrealDiffDetailTreeNode::GetValueText()
{
	
	return FText();
}

FText FUnrealDiffDetailTreeNode::GetDisplayNameText()
{
	return FText();
}

FString FUnrealDiffDetailTreeNode::GetUniqueNodeId() const
{
	return NodeId + GetParentUniqueNodeId();
}

FString FUnrealDiffDetailTreeNode::GetParentUniqueNodeId() const
{
	return FString();
}

FName FUnrealDiffDetailTreeNode::GetCategoryName()
{
	if (Property.IsValid() && IsContainerNode())
	{
		return Property->GetFName();
	}

	return NAME_None;
}

bool FUnrealDiffDetailTreeNode::HasAnyDifferenceRecurse()
{
	bool Ret = bHasAnyDifference;
	if (Children.Num() > 0)
	{
		for (const auto& TreeNode : Children)
		{
			if (TreeNode->HasAnyDifferenceRecurse())
			{
				Ret = true;
			}
		}		
	}
	
	return Ret;
}

const TArray<TSharedPtr<FUnrealDiffDetailTreeNode>>& FUnrealDiffDetailTreeNode::GetChildNodes()
{
	return Children;
}

FText FUnrealDiffDetailTreeNode::GetValueTextStructInternal(const FProperty* InProperty)
{
	const auto StructData = GetStructData(0);
	// void* RowData = InProperty->ContainerPtrToValuePtr<void>(StructData, 0)
	return GetValueTextInternal(StructData, InProperty);
}

FText FUnrealDiffDetailTreeNode::GetValueTextContainerInternal(void* PropertyData, const FProperty* InProperty)
{
	return GetValueTextInternal(PropertyData, InProperty);
}

FText FUnrealDiffDetailTreeNode::GetValueTextInternal(void* ValueAddress, const FProperty* InProperty)
{
	FText OutText;
	if (CastField<const FStructProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FMapProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FSetProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FArrayProperty>(InProperty))
	{
		return OutText;
	}
	
	EDataTableExportFlags DTExportFlags = EDataTableExportFlags::UseJsonObjectsForStructs;
	OutText = DataTableUtils::GetPropertyValueAsText(InProperty, (uint8*)ValueAddress);
	if (OutText.IsEmpty())
	{
		OutText = FText::FromString("None");
	}
	
	return OutText;
}

