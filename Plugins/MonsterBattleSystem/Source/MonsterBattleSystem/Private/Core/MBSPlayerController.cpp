// © Argo. All rights reserved.

#include "Core/MBSPlayerController.h"

#include "MonsterBattleSystem.h"
#include "Core/Items/MBSItem.h"

void AMBSPlayerController::BeginPlay()
{
	// Load inventory off save game before starting play
	LoadInventory();

	Super::BeginPlay();
}

bool AMBSPlayerController::AddInventoryItem(UMBSItem* NewItem, const int32 ItemCount, const int32 ItemLevel)
{
	bool bChanged{false};
	if (!NewItem)
	{
		UE_LOG(LogMonsterBattleSystem, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"))
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		UE_LOG(LogMonsterBattleSystem, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName())
		return false;
	}

	// Find current item data, which may be empty
	FMBSItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	// Find modified data
	FMBSItemData NewData{OldData};
	NewData.UpdateItemData(FMBSItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		//If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}

	// if (bAutoSlot)

	if (bChanged)
	{
		// If anything changed, write to save game
		SaveInventory();
		return true;
	}

	return false;
}

bool AMBSPlayerController::RemoveInventoryItem(UMBSItem* RemovedItem, const int32 RemoveCount)
{
	if (!RemovedItem)
	{
		UE_LOG(LogMonsterBattleSystem, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"))
		return false;
	}

	// Find current item data, which may be empty
	FMBSItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

	// If RemoveCount <= 0, delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// Remove item entirely, make sure it is unslot
		InventoryData.Remove(RemovedItem);

		// for (TPair<FMBSItemSlot, UMBSItem*>& Pair : SlottedItems)
		// ...
	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(false, RemovedItem);

	SaveInventory();
	return true;
}

void AMBSPlayerController::GetInventoryItems(TArray<UMBSItem*>& Items, const FPrimaryAssetType ItemType)
{
	for (const TPair<UMBSItem*, FMBSItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId{Pair.Key->GetPrimaryAssetId()};

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

int32 AMBSPlayerController::GetInventoryItemCount(const UMBSItem* Item) const
{
	const FMBSItemData* FoundItem{InventoryData.Find(Item)};

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}

	return 0;
}

bool AMBSPlayerController::GetInventoryItemData(const UMBSItem* Item, FMBSItemData& ItemData) const
{
	const FMBSItemData* FoundItem{InventoryData.Find(Item)};

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}

	ItemData = FMBSItemData(0, 0);
	return false;
}

bool AMBSPlayerController::SaveInventory() const
{
	return false;
}

bool AMBSPlayerController::LoadInventory() const
{
	return false;
}

void AMBSPlayerController::NotifyInventoryItemChanged(const bool bAdded, UMBSItem* Item)
{
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	// Call BP update event
	InventoryItemChanged(bAdded, Item);
}

void AMBSPlayerController::NotifyInventoryLoaded() const
{
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}

// void AMBSPlayerController::HandleSaveGameLoaded(UMBSSaveGame* NewSaveGame)
// {
// 	LoadInventory();
// }