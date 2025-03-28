// © Argo. All rights reserved.

#include "UI/MBSListItem.h"

void UMBSListItem::AddNewItemToInventoryList(const FString InItemName, const int InItemCount) const
{
	if (ItemName && ItemCount)
	{
		ItemName->SetText(FText::FromString(InItemName));
		ItemCount->SetText(FText::FromString(FString::FromInt(InItemCount)));
	}
}

void UMBSListItem::SetItemCount(const int NewCount) const
{
	if (ItemCount)
	{
		ItemCount->SetText(FText::FromString(FString::FromInt(NewCount)));
	}
}
