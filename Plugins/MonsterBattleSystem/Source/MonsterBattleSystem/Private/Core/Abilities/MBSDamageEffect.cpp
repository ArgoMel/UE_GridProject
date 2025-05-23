// © Argo. All rights reserved.

#include "Core/Abilities/MBSDamageEffect.h"

#include "GameplayTagsManager.h"
#include "Core/Abilities/MBSAttributeSet.h"

UMBSDamageEffect::UMBSDamageEffect()
{
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UMBSAttributeSet::GetDamageAttribute();
	DamageInfo.ModifierOp = EGameplayModOp::Override;
	DamageInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(35.f);
	Modifiers.Add(DamageInfo);

	FGameplayEffectCue DamageCue;
	FGameplayTagContainer DamageTagContainer;
	TArray<FString> Tags;
	Tags.Add("GameplayCue.Punched");
	UGameplayTagsManager::Get().RequestGameplayTagContainer(Tags, DamageTagContainer);
	DamageCue.GameplayCueTags = DamageTagContainer;
	GameplayCues.Add(DamageCue);
}
