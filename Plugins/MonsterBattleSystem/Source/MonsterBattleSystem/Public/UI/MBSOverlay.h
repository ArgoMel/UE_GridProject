// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/MBSData.h"
#include "MBSOverlay.generated.h"

class UMBSListItem;
class UMBSItem;
class AMBSPlayerController;
class UTextBlock;
class UScrollBox;

/**
 * Main HUD for Hopper
 */
UCLASS()
class MONSTERBATTLESYSTEM_API UMBSOverlay : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> CoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UScrollBox> InventoryList;

	UPROPERTY(BlueprintReadOnly,meta = (AllowPrivateAccess = true))
	TObjectPtr<AMBSPlayerController> PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<UMBSListItem> MBSListItemClass;

	FOnInventoryItemChangedNative InventoryItemChangedNative;

private:
	void UpdateInventoryInformation(bool bAdded, UMBSItem* Item);
};
