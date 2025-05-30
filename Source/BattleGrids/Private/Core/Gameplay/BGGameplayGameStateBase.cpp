// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Gameplay/BGGameplayGameStateBase.h"

#include "Actors/BGActor.h"
#include "Characters/BGCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ABGGameplayGameStateBase::ABGGameplayGameStateBase()
{
}

TArray<ABGActor*> ABGGameplayGameStateBase::GetSpawnedActorsOfType(EBGActorType const& ActorType)
{
	TArray<ABGActor*> AllSpawnedActorsOfType;
	
	for (auto Actor : AllBGActors)
	{
		if (Actor->GetActorType() == ActorType && !Actor->IsPendingKillPending())
		{
			AllSpawnedActorsOfType.Add(Actor);
		}
	}

	return AllSpawnedActorsOfType;
}


void ABGGameplayGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	// RefreshCurrentWorldState();
}

void ABGGameplayGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGGameplayGameStateBase, AllBGActors)
	DOREPLIFETIME(ABGGameplayGameStateBase, AllBGCharacters)
}

void ABGGameplayGameStateBase::RefreshCurrentWorldState_Implementation()
{
	/** populate AllBGActors array with all ABGActor actors in the world */
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABGActor::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		auto const BGActor = Cast<ABGActor>(Actor);
		if (BGActor && !BGActor->IsPendingKillPending())
		{
			AllBGActors.AddUnique(BGActor);
		}
	}

	/** populate AllBGCharacters array with all ABGCharacter characters in the world */
	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(this, ABGCharacter::StaticClass(), Characters);

	for (auto Character : Characters)
	{
		auto const BGCharacter = Cast<ABGCharacter>(Character);
		if (BGCharacter && !BGCharacter->IsPendingKillPending())
		{
			AllBGCharacters.AddUnique(BGCharacter);
		}
	}
}

TArray<ABGCharacter*> ABGGameplayGameStateBase::GetSpawnedCharacters()
{
	return AllBGCharacters;
}

void ABGGameplayGameStateBase::AddSpawnedEntityToArray(ABGActor* ActorToAdd)
{
	if (ActorToAdd)
	{
		AllBGActors.AddUnique(ActorToAdd);
	}
}

void ABGGameplayGameStateBase::AddSpawnedEntityToArray(ABGCharacter* CharacterToAdd)
{
	if (CharacterToAdd)
	{
		AllBGCharacters.AddUnique(CharacterToAdd);
	}
}