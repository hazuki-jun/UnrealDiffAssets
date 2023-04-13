#pragma once

/** Cached string table entry */
struct FCachedStringTableEntry
{
	FCachedStringTableEntry(FString InKey, FString InSourceString)
		: Key(MoveTemp(InKey))
		, SourceString(MoveTemp(InSourceString))
		, RowState(0)
	{
	}

	FString Key;
	FString SourceString;
	int32 RowState;
};
