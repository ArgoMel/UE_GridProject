// © Argo. All rights reserved.

#include "UI/MBSOverlay.h"
#include "Components/ScrollBox.h"
#include "Core/Items/MBSItem.h"
#include "Components/TextBlock.h"
#include "Core/MBSPlayerController.h"
#include "UI/MBSListItem.h"

void UMBSOverlay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayerController=Cast<AMBSPlayerController>(GetOwningPlayer());
	// set delegate to update UI here (PlayerController will be valid by this point)
	if (PlayerController)
	{
		PlayerController->OnInventoryItemChangedNative.AddUObject(
			this, &UMBSOverlay::UpdateInventoryInformation);
	}
}

void UMBSOverlay::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMBSOverlay::UpdateInventoryInformation(bool bAdded, UMBSItem* Item)
{
	if (PlayerController)
	{
		FMBSItemData ItemData;
		if (PlayerController->GetInventoryItemData(Item, ItemData))
		{
			// Increase coin collection count
			CoinCount->SetText(FText::FromString(FString::FromInt(ItemData.ItemCount)));

			// Add to inventory list
			UMBSListItem* NewListItem = CreateWidget<UMBSListItem>(
				this, MBSListItemClass.LoadSynchronous());
			if (NewListItem)
			{
				// Check if we already have items in the UI
				if (InventoryList->HasAnyChildren())
				{
					TArray<UWidget*> ArrayOfChildren = InventoryList->GetAllChildren();
					for (UWidget* Widget : ArrayOfChildren)
					{
						const UMBSListItem* ListItem = Cast<UMBSListItem>(Widget);
						if (ListItem)
						{
							if (ListItem->GetItemName().EqualToCaseIgnored(Item->ItemName))
							{
								ListItem->SetItemCount(ItemData.ItemCount);
								break;
							}
						}
					}
				}
				// Add a new item to the UI
				else
				{
					NewListItem->AddNewItemToInventoryList(Item->ItemName.ToString(), ItemData.ItemCount);
					InventoryList->AddChild(NewListItem);
				}
			}
		}
	}
}
