// © Argo. All rights reserved.

#pragma once

#include "Core/MBSAssetManager.h"
#include "Core/Items/MBSItem.h"
#include "MBSTokenItem.generated.h"

/** Native base class for tokens/currency, should be blueprinted */
UCLASS()
class MONSTERBATTLESYSTEM_API UMBSTokenItem : public UMBSItem
{
	GENERATED_BODY()
public:
	/** Constructor */
	UMBSTokenItem()
	{
		ItemType = {TEXT("Token")};
		MaxCount = 0; // Infinite
	}
};
