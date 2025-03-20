// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/MBSData.h"
#include "MonsterStatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MONSTERBATTLESYSTEM_API UMonsterStatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMonsterStatComponent();
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FMonsterStats MonsterStats;

public:
	FMonsterStats GetMonsterStats() const { return MonsterStats; }
	FMonsterStats& ModifyMonsterStats() { return MonsterStats; }
};
