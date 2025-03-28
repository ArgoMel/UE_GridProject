// © Argo. All rights reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "MBSAbilitySystemComponent.generated.h"

UCLASS()
class MONSTERBATTLESYSTEM_API UMBSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UMBSAbilitySystemComponent();
	
public:
	/** Version of function in AbilitySystemGlobals that returns correct type */
	static UMBSAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
	
};
