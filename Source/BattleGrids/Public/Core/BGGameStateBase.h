// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameStateBase.h"
#include "BGGameStateBase.generated.h"

class ABGBoard;

/**
 * Parent GameStateBase class for BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API ABGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};