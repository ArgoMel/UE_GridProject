// © Argo. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "MBSItem.generated.h"

class UMBSGameplayAbility;

/** Base class for all items, do not blueprint directly */
UCLASS(Abstract, BlueprintType)
class MONSTERBATTLESYSTEM_API UMBSItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/** Constructor */
	UMBSItem()
		: Price(0),
		  MaxCount(1),
		  MaxLevel(1),
		  AbilityLevel(1)
	{
	}
	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
public:
	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Item")
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Item")
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Item")
	FSlateBrush ItemIcon;

	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Item")
	int32 Price;

	/** Maximum number of instances that can be in the inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Max")
	int32 MaxCount;

	/** Maximum level this item can be, <= means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Max")
	int32 MaxLevel;

	/** Ability to grant from this item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UMBSGameplayAbility> GrantedAbility;

	/** Ability level this item grants. <= means the character level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	int32 AbilityLevel;

public:
	/** Returns if the item is consumable (MaxCount <= 0) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	bool IsConsumable() const;
	
	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = "Item")
	FString GetIdentifierString() const;
};
