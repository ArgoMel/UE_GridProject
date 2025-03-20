// © Argo. All rights reserved.

#include "Actors/MBSMonster.h"

#include "Core/Components/MonsterStatComponent.h"

AMBSMonster::AMBSMonster()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MonsterStats = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("Monster Stats"));
}

void AMBSMonster::BeginPlay()
{
	Super::BeginPlay();
}
