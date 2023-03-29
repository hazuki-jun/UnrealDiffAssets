#pragma once

#include "CoreMinimal.h"

class UnrealDataTableDetailTreeNode
{
public:
	UnrealDataTableDetailTreeNode();
	virtual ~UnrealDataTableDetailTreeNode();
	virtual TSharedRef< ITableRow > GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) = 0;
};
