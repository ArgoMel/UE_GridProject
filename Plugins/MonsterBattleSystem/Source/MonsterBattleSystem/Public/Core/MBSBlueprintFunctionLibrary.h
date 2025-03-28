// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MBSBlueprintFunctionLibrary.generated.h"

/**
 * Hopper Helper Functions
 */
UCLASS()
class MONSTERBATTLESYSTEM_API UMBSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * Compares the provided Velocity's Z value with the provided VelocityToKill float.
	 * Function normalizes the Velocity if it isn't already.
	 * @param Velocity Velocity of Actor which hit
	 * @param VelocityToKill Z-Velocity required to cause a squash
	 * @return True if impact counts as a squash.
	 */
	UFUNCTION(BlueprintPure)
	static bool IsSquashingHit(const FVector Velocity, const float VelocityToKill = 0.f);

	/**
	 * Launches Target away from the provided FromLocation using the provided AttackForce.
	 * @param FromLocation Location of attacker or cause of launch
	 * @param Target Character to be launched
	 * @param AttackForce Force applied to the launch
	 */
	UFUNCTION(BlueprintCallable)
	static void ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target, const float AttackForce);

	/** Game Input Only, with custom boolean to prevent mouse consume */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Input")
	static void SetInputMode_GameOnly_MouseConsume(APlayerController* PlayerController, bool ConsumeCaptureMouseDown);
};
