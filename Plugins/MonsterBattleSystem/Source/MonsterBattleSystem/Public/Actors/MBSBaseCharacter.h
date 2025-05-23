// © Argo. All rights reserved.

#pragma once

#include "Core/MBSData.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "PaperCharacter.h"
#include "Core/Interfaces/MBSCharacterInterface.h"
#include "MBSBaseCharacter.generated.h"

class UMBSGameplayAbility;
class UGameplayEffect;
class UMBSAbilitySystemComponent;
class UMBSAttributeSet;
class UAIPerceptionComponent;
class USphereComponent;

/**
 * Base character class
 */
UCLASS()
class MONSTERBATTLESYSTEM_API AMBSBaseCharacter : public APaperCharacter, public IAbilitySystemInterface, public IMBSCharacterInterface
{
	GENERATED_BODY()
	/**********************************
	*         Class Overrides
	**********************************/
public:
	/** Friended to allow access to handle functions */
	friend UMBSAttributeSet;
	AMBSBaseCharacter();
protected:
	virtual void BeginPlay() override;
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	/**
	* Launches Target away from the provided FromLocation using the provided AttackForce.
	* @param FromLocation Location of attacker or cause of launch
	* @param InAttackForce Force applied to the launch
	*/
	UFUNCTION(BlueprintCallable)
	virtual void ApplyPunchForceToCharacter(const FVector FromLocation, const float InAttackForce) const override;

public:
	/* Broadcast when the Flipbook animation is walking */
	UPROPERTY(BlueprintAssignable, Category="Delegates")
	FOnFootstepTaken OnFootstepTaken;
	FOnFootstepTakenNative OnFootstepTakenNative;

	/* Broadcast when Character's health reaches 0 */
	UPROPERTY(BlueprintAssignable, Category="Delegates")
	FOnCharacterDeath OnCharacterDeath;
	FOnCharacterDeathNative OnCharacterDeathNative;

	/* Broadcast when Character's attack is finished */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnAttackTimerEnd OnAttackTimerEnd;
	FOnAttackTimerEndNative OnAttackTimerEndNative;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMBSAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> AttackSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** Abilities to grant to this character on creation. These will be activated
	 * by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UMBSGameplayAbility>> GameplayAbilities;

	UPROPERTY()
	TObjectPtr<UMBSAttributeSet> Attributes;

	/** If true we have initialized our abilities */
	UPROPERTY()
	uint8 bAbilitiesInitialized:1;

	/**************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	EMBSAnimationDirection CurrentAnimationDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FMBSMovementFlipbooks MovementFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FMBSPunchFlipbooks PunchFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<float> JumpPowerLevels{1200.f, 1400.f, 1800.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float AttackForce{750.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float AttackRadius{150.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bIsMoving:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bAttackGate:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bFootstepGate:1;

	FTimerHandle AttackTimer;
	FTimerHandle FootstepTimer;
	FTimerHandle JumpReset;
	int JumpCounter{};

	FGameplayTag DeadTag;
	
public:
	/**********************************
	 *           Getters
	 **********************************/

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

protected:
	/**********************************
	 *         Ability System
	 **********************************/
	virtual void AddStartupGameplayAbilities();

	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AMBSBaseCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/** Called from MBSAttributeSet, these call BP events above */

	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AMBSBaseCharacter* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**********************************
	 *            Combat
	 **********************************/

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Actions")
	void HandlePunch();

	/**
	 * Plays a punch Flipbook from the character's PunchFlipbooks struct
	 * based on the CurrentAnimationDirection enum, then sets the AttackTimer
	 * to the provided float value.
	 * @param TimerValue How much time it takes before another attack can execute.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Animation")
	void PlayPunchAnimation(const float TimerValue = 0.3f);

	/**********************************
	 *       Delegates & Events
	 **********************************/

	/**
	 * Called upon movement, with a timer delay of 0.3 seconds.
	 * Use for footstep sound or particle effects in Blueprints.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFootstep();
	void OnFootstepNative();

	/**
	 * Called when Character dies.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();
	void OnDeathNative();

	/**
	 * Called when the Attack Timer ends and bAttackGate is open again.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackEnd();
	void OnAttackEndNative();

	/**********************************
	 *           Movement
	 **********************************/
	void ModifyJumpPower();
	void ResetJumpPower();
	
	/**********************************
	 *           Animation
	 **********************************/

	/**
	 * Animates the sprite with Editor-set Flipbooks for movement. This function is called
	 * by binding it to the OnCharacterMovementUpdated delegate. Direction is selected by
	 * calling SetCurrentAnimationDirection().
	 * @param DeltaTime Time since last frame.
	 * @param OldLocation Location at call.
	 * @param OldVelocity Velocity at call.
	 */
	UFUNCTION()
	void Animate(float DeltaTime, FVector OldLocation, const FVector OldVelocity);

	/**
	 * Sets the CurrentAnimationDirection enum by detecting velocity and the Player's camera rotation
	 * in world space via the provided ViewInfo. If ViewInfo is not set, the function returns early.
	 * @param Velocity Actor's velocity at time of call.
	 * @param ViewInfo Player's camera information.
	 */
	virtual void SetCurrentAnimationDirection(const FVector& Velocity, TOptional<FMinimalViewInfo> ViewInfo);
};
