// © 2021 Matthew Barham. All Rights Reserved.

#include "Core/BGGameStateBase.h"

#include "Net/UnrealNetwork.h"

void ABGGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}