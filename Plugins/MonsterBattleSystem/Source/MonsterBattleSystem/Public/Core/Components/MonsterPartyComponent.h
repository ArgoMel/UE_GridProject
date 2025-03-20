// © Argo. All rights reserved.

#pragma once

#include "Core/MBSData.h"
#include "Components/ActorComponent.h"
#include "MonsterPartyComponent.generated.h"

class AMBSMonster;
struct FMonsterStats;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MONSTERBATTLESYSTEM_API UMonsterPartyComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMonsterPartyComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<TSubclassOf<AMBSMonster>, FMonsterStats> MonsterParty;
};
