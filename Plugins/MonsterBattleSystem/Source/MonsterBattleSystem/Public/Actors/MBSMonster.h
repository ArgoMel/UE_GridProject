// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MBSBaseCharacter.h"
#include "MBSMonster.generated.h"

class UMonsterStatComponent;

UCLASS()
class MONSTERBATTLESYSTEM_API AMBSMonster : public AMBSBaseCharacter
{
	GENERATED_BODY()
public:
	AMBSMonster();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UMonsterStatComponent> MonsterStats;
};
