// © Argo. All rights reserved.

#include "Core/HopperAssetManager.h"
#include "Core/Items/HopperItem.h"
#include "AbilitySystemGlobals.h"
#include "MonsterBattleSystem.h"

// initialize static variable
const FPrimaryAssetType UHopperAssetManager::TokenItemType {TEXT("Token")};

void UHopperAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

UHopperAssetManager& UHopperAssetManager::Get()
{
	UHopperAssetManager* This = Cast<UHopperAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	UE_LOG(LogMonsterBattleSystem, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be HopperAssetManager!"))
	return *NewObject<UHopperAssetManager>(); // never calls this
}

UHopperItem* UHopperAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning) const
{
	const FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UHopperItem* LoadedItem = Cast<UHopperItem>(ItemPath.TryLoad());

	if (bLogWarning && !LoadedItem)
	{
		UE_LOG(LogMonsterBattleSystem, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString())
	}

	return LoadedItem;
}
