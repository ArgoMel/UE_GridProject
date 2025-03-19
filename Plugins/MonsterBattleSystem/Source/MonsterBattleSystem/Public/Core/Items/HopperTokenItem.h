// © Argo. All rights reserved.

#pragma once

#include "Core/HopperAssetManager.h"
#include "Core/Items/HopperItem.h"
#include "HopperTokenItem.generated.h"

/** Native base class for tokens/currency, should be blueprinted */
UCLASS()
class MONSTERBATTLESYSTEM_API UHopperTokenItem : public UHopperItem
{
	GENERATED_BODY()
public:
	/** Constructor */
	UHopperTokenItem()
	{
		ItemType = UHopperAssetManager::TokenItemType;
		MaxCount = 0; // Infinite
	}
};
