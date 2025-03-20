// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MBSGameInstanceInterface.generated.h"

class AMBSMonster;
struct FMonsterStats;

UINTERFACE()
class UMBSGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

class MONSTERBATTLESYSTEM_API IMBSGameInstanceInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SaveMonsterStats(TSubclassOf<AMBSMonster> InMonster, const FMonsterStats& InMonsterStats);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SaveMonsterParty(const TMap<TSubclassOf<AMBSMonster>, FMonsterStats>& InMonsterParty);
};
