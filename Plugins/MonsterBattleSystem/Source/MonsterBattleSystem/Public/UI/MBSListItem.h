// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MBSListItem.generated.h"

class UMBSItem;
class UTextBlock;

UCLASS()
class MONSTERBATTLESYSTEM_API UMBSListItem : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ItemCount;

public:
	void AddNewItemToInventoryList(const FString InItemName, const int InItemCount) const;

	FText GetItemName() const { return ItemName->GetText(); }
	FText GetItemCount() const { return ItemCount->GetText(); }

	void SetItemCount(int NewCount) const;
};
