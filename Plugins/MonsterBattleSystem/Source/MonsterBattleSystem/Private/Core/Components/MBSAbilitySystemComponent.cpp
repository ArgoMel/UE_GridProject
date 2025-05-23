// © Argo. All rights reserved.

#include "Core/Components/MBSAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"

UMBSAbilitySystemComponent::UMBSAbilitySystemComponent()
{
}

UMBSAbilitySystemComponent* UMBSAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UMBSAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
