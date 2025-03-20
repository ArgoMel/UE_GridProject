// © Argo. All rights reserved.

#pragma once

#include "Core/MBSData.h"
#include "GameFramework/SaveGame.h"
#include "MonsterPartySave.generated.h"

struct FMonsterStats;
class AMBSMonster;

/**
 * Save for monsters in a player or AI's party
 */
UCLASS()
class MONSTERBATTLESYSTEM_API UMonsterPartySave : public USaveGame
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<TSubclassOf<AMBSMonster>, FMonsterStats> MonsterParty;
};
