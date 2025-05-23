// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBSData.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/MBSInventoryInterface.h"
#include "MBSPlayerController.generated.h"

struct FMBSItemData;
class UMBSItem;
/**
 * MBS Player Controller for Gameplay
 */
UCLASS()
class MONSTERBATTLESYSTEM_API AMBSPlayerController : public APlayerController, public IMBSInventoryInterface
{
	GENERATED_BODY()
public:
	/** Constructor and overrides */
	AMBSPlayerController()
	{
	}
protected:
	virtual void BeginPlay() override;

public:
	/*******************************************
	*    Implement IMBSInventoryInterface 
	*******************************************/
	virtual const TMap<UMBSItem*, FMBSItemData>& GetInventoryDataMap() const override
	{
		return InventoryData;
	}

	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}

	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return OnInventoryLoadedNative;
	}
	
public:
	/*******************************************
	 *               Inventory
	 ******************************************/

	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory")
	TMap<UMBSItem*, FMBSItemData> InventoryData;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemChanged OnInventoryItemChanged;

	/** Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

public:
	/** Called after the inventory was changed and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void InventoryItemChanged(bool bAdded, UMBSItem* Item);
	
	/** Adds a new inventory item.
	 * If the item supports count you can add more than one count.
	 * It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItem(UMBSItem* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1);

	/** Remove an inventory item. A remove count of <= 0 means to remove all copies */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveInventoryItem(UMBSItem* RemovedItem, int32 RemoveCount = 1);

	/** Returns all inventory items of a given type.
	 * If none is passed as type it will return all */
	void GetInventoryItems(TArray<UMBSItem*>& Items, FPrimaryAssetType ItemType);

	/** Returns number of instances of this item found in the inventory.
	 * This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetInventoryItemCount(const UMBSItem* Item) const;

	/** Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetInventoryItemData(const UMBSItem* Item, FMBSItemData& ItemData) const;

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SaveInventory() const;

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool LoadInventory() const;

protected:
	/** Calls the inventory update callbacks */
	void NotifyInventoryItemChanged(bool bAdded, UMBSItem* Item);
	// void NotifySlottedItemChanged(FMBSItemData ItemSlot, UMBSItem* Item);
	void NotifyInventoryLoaded() const;

	/** Called when a global save game as been loaded */
	// void HandleSaveGameLoaded(UMBSSaveGame* NewSaveGame);
};
