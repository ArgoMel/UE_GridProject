// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Components/MonsterStatComponent.h"
#include "GameFramework/SaveGame.h"
#include "MonsterSave.generated.h"

class AMBSMonster;

/**
 * Save file for Monsters
 */
UCLASS()
class MONSTERBATTLESYSTEM_API UMonsterSave : public USaveGame
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMBSMonster> MonsterReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMonsterStats MonsterStats;
};
