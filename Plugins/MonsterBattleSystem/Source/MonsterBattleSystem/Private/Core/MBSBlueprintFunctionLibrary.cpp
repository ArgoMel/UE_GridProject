// © Argo. All rights reserved.

#include "Core/MBSBlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UMBSBlueprintFunctionLibrary::IsSquashingHit(const FVector Velocity, const float VelocityToKill)
{
	return (Velocity.IsNormalized() ? Velocity.Z : Velocity.GetSafeNormal().Z) > VelocityToKill;
}

void UMBSBlueprintFunctionLibrary::ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target, const float AttackForce)
{
	if (IsValid(Target))
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation, TargetLocation);

		Target->GetCharacterMovement()->Launch(FVector(
			Direction.X * AttackForce,
			Direction.Y * AttackForce,
			abs(Direction.Z + 1) * AttackForce));
	}
}

void UMBSBlueprintFunctionLibrary::SetInputMode_GameOnly_MouseConsume(APlayerController* PlayerController, bool ConsumeCaptureMouseDown)
{
	if (PlayerController != nullptr)
	{
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(ConsumeCaptureMouseDown);
		PlayerController->SetInputMode(InputMode);
	}
#if WITH_EDITOR
	else
	{
		FMessageLog("PIE").Error(FText::FromString("SetInputMode_GameOnly_MouseConsume expects a valid player controller as 'PlayerController' target"));
	}
#endif // WITH_EDITOR
}
