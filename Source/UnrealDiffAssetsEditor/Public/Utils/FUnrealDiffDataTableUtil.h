#pragma once

#include "CoreMinimal.h"

struct FUnrealDiffDataTableUtil
{
	/**
	 * @brief 导出DataTable指定行
	 * @param ValueStr [Out]
	 * @param DataTable 
	 * @param RowName 
	 */
	static void ExportRowText(FString& ValueStr, class UDataTable* DataTable, const FName& RowName);

	/**
	 * @brief 导出DataTable指定行 只导出Value
	 * @param ValueStr [Out]
	 * @param DataTable 
	 * @param RowName 
	 */
	static void ExportRowValueText(FString& ValueStr, class UDataTable* DataTable, const FName& RowName);
	
	/**
	 * @brief 合并表中的一行 目标表不存在行是会新添加一行
	 * @param DestDataTable 
	 * @param SrcDataTable 
	 * @param RowName 
	 */
	static void MergeRow(class UDataTable* DestDataTable, class UDataTable* SrcDataTable, const FName& RowName);

	/**
	 * @brief 新添加一行到指定表
	 * @param DestDataTable 
	 * @param RowName 
	 */
	static void AddRow(class UDataTable* DestDataTable, const FName& RowName);
	
	/**
	 * @brief 从指定表删除指定行
	 * @param DataTable 
	 * @param RowName 
	 */
	static void DeleteRow(class UDataTable* DataTable, FName RowName);

	/**
	 * @brief 指定Row是否存在差异
	 * @param InLocalDataTable 
	 * @param InRemoteDataTable 
	 * @param RowName 
	 * @return 
	 */
	static bool HasAnyDifferenceRowToRow(UDataTable* InLocalDataTable, UDataTable* InRemoteDataTable, FName& RowName, bool bIgnorePropertyName = true);
};
