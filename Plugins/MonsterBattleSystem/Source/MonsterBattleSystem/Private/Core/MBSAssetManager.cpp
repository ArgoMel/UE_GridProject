// © Argo. All rights reserved.

#include "Core/MBSAssetManager.h"
#include "Core/Items/MBSItem.h"
#include "AbilitySystemGlobals.h"
#include "MonsterBattleSystem.h"

// initialize static variable
const FPrimaryAssetType UMBSAssetManager::TokenItemType {TEXT("Token")};

void UMBSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

UMBSAssetManager& UMBSAssetManager::Get()
{
	UMBSAssetManager* This = Cast<UMBSAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	UE_LOG(LogMonsterBattleSystem, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be HopperAssetManager!"))
	return *NewObject<UMBSAssetManager>(); // never calls this
}

UMBSItem* UMBSAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning) const
{
	const FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UMBSItem* LoadedItem = Cast<UMBSItem>(ItemPath.TryLoad());

	if (bLogWarning && !LoadedItem)
	{
		UE_LOG(LogMonsterBattleSystem, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString())
	}

	return LoadedItem;
}
