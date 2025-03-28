// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MBSCharacterInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMBSCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Character functions abstracted through interface calls
 */
class MONSTERBATTLESYSTEM_API IMBSCharacterInterface
{
	GENERATED_BODY()
public:
	virtual void ApplyPunchForceToCharacter(const FVector FromLocation, const float InAttackForce) const = 0;
};
