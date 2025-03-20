// © Argo. All rights reserved.

#pragma once

#include "Engine/AssetManager.h"
#include "MBSAssetManager.generated.h"

class UMBSItem;

UCLASS()
class MONSTERBATTLESYSTEM_API UMBSAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	/** Constructor and overrides */
	UMBSAssetManager() {}
	virtual void StartInitialLoading() override;
	
public:
	/** Static types for items */
	static const FPrimaryAssetType TokenItemType;
	
public:
	/** Returns the current AssetManager object */
	static UMBSAssetManager& Get();

	/**
	 * Synchronously loads a HopperItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bLogWarning If true, this will log a warning if the item failed to load
	 */
	UMBSItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true) const;
};
