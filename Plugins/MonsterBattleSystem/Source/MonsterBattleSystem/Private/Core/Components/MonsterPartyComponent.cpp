// © Argo. All rights reserved.

#include "Core/Components/MonsterPartyComponent.h"

// Sets default values for this component's properties
UMonsterPartyComponent::UMonsterPartyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterPartyComponent::BeginPlay()
{
	Super::BeginPlay();
}