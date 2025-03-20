// © Argo. All rights reserved.

#include "Core/Items/MBSItem.h"

FPrimaryAssetId UMBSItem::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(ItemType, GetFName());
}

bool UMBSItem::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}
	return false;
}

FString UMBSItem::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}
