// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/MBSData.h"
#include "Abilities/GameplayAbility.h"
#include "MBSGameplayAbility.generated.h"

/**
 * Hopper Gameplay Ability
 */
UCLASS()
class MONSTERBATTLESYSTEM_API UMBSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UMBSGameplayAbility();
	
public:
	// Abilities will activate when input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EMBSAbilityInputID AbilityInputID {EMBSAbilityInputID::None};
};
