// © Argo. All rights reserved.

#include "Actors/MBSBaseCharacter.h"

#include "MonsterBattleSystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "PaperFlipbookComponent.h"
#include "Core/Abilities/MBSAttributeSet.h"
#include "Core/Abilities/MBSGameplayAbility.h"
#include "Core/Components/MBSAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

AMBSBaseCharacter::AMBSBaseCharacter()
{
	bReplicates = true;

	bAbilitiesInitialized = false;
	bFootstepGate = true;
	bAttackGate = true;

	OnCharacterMovementUpdated.AddDynamic(this, &AMBSBaseCharacter::Animate);

	GetCapsuleComponent()->SetCapsuleRadius(70.f);

	GetSprite()->SetRelativeScale3D(FVector(11.f, 11.f, 11.f));
	GetSprite()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetSprite()->SetUsingAbsoluteRotation(true);
	GetSprite()->SetFlipbook(MovementFlipbooks.IdleDown);
	GetSprite()->CastShadow = true;
	
	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetSphereRadius(AttackRadius);

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerSphere->SetupAttachment(RootComponent);
	TriggerSphere->SetSphereRadius(130.f);

	GetCharacterMovement()->GravityScale = 2.8f;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];
	
	AbilitySystemComponent = CreateDefaultSubobject<UMBSAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UMBSAttributeSet>(TEXT("Attributes"));

	DeadTag = FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead");
}

void AMBSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	OnFootstepTakenNative.AddUObject(this, &AMBSBaseCharacter::OnFootstepNative);
	OnAttackTimerEndNative.AddUObject(this, &AMBSBaseCharacter::OnAttackEndNative);
	OnCharacterDeathNative.AddUObject(this, &AMBSBaseCharacter::OnDeathNative);
}

void AMBSBaseCharacter::OnJumped_Implementation()
{
	GetCharacterMovement()->bNotifyApex = true;

	++JumpCounter;
	GetWorldTimerManager().ClearTimer(JumpReset);

	Super::OnJumped_Implementation();
}

void AMBSBaseCharacter::Landed(const FHitResult& Hit)
{
	GetCharacterMovement()->GravityScale = 2.8f;

	if (JumpCounter > 2)
	{
		ResetJumpPower();
	}
	ModifyJumpPower();
	GetWorldTimerManager().SetTimer(JumpReset, this, &AMBSBaseCharacter::ResetJumpPower, 0.2f, false);

	Super::Landed(Hit);
}

void AMBSBaseCharacter::NotifyJumpApex()
{
	GetCharacterMovement()->GravityScale = 5.f;
	Super::NotifyJumpApex();
}

void AMBSBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server GAS init
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

void AMBSBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// called in SetupPlayerInputComponent() for redundancy
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			FTopLevelAssetPath("/Script/MonsterBattleSystem.EMBSAbilityInputID"),
			static_cast<int32>(EMBSAbilityInputID::Confirm),
			static_cast<int32>(EMBSAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AMBSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// called in OnRep_PlayerState() for redundancy
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			FTopLevelAssetPath("/Script/MonsterBattleSystem.EMBSAbilityInputID"),
			static_cast<int32>(EMBSAbilityInputID::Confirm),
			static_cast<int32>(EMBSAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AMBSBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMBSBaseCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (UKismetSystemLibrary::DoesImplementInterface(Other, UMBSCharacterInterface::StaticClass())
		&& this-> ActorHasTag("Enemy"))
	{
		Cast<IMBSCharacterInterface>(Other)->ApplyPunchForceToCharacter(GetActorLocation(), 100.f);
	}
}

void AMBSBaseCharacter::ModifyJumpPower()
{
	switch (JumpCounter)
	{
	case 1:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[1];
		UE_LOG(LogMonsterBattleSystem, Display, TEXT("Jump Power Level 1"))
		break;
	case 2:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[2];
		UE_LOG(LogMonsterBattleSystem, Display, TEXT("Jump Power Level 2"))
		break;
	default:
		UE_LOG(LogMonsterBattleSystem, Display, TEXT("Jump Power Level 0"))
		break;
	}
}

void AMBSBaseCharacter::ResetJumpPower()
{
	JumpCounter = 0;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];
	UE_LOG(LogMonsterBattleSystem, Display, TEXT("Jump Power Reset"))
}

void AMBSBaseCharacter::HandlePunch_Implementation()
{
	TArray<AActor*> ActorsArray;
	AttackSphere->GetOverlappingActors(ActorsArray);
	int Count{};

	if (ActorsArray.Num() > 0)
	{
		for (AActor* Actor : ActorsArray)
		{
			if (Actor
				&& Actor != this
				&& Actor->ActorHasTag("Enemy")
				&& Actor->Implements<UMBSCharacterInterface>()
				&& Actor->Implements<UAbilitySystemInterface>())
			{
				// don't punch if dead
				if (Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead")))
				{
					UE_LOG(LogMonsterBattleSystem, Log, TEXT("Found IsDead"))
					continue;
				}

				UE_LOG(LogMonsterBattleSystem, Log, TEXT("Applying Punch Force"))
				Cast<IMBSCharacterInterface>(Actor)->ApplyPunchForceToCharacter(GetActorLocation(), AttackForce);

				const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.Hit");
				FGameplayEventData Payload = FGameplayEventData();
				Payload.Instigator = GetInstigator();
				Payload.Target = Actor;
				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Actor);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);

				++Count;
			}
		}
	}

	// if our Count returns 0, it means we did not hit an enemy, and we should end our ability
	if (Count == 0)
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.NoHit");
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);
	}
}

void AMBSBaseCharacter::ApplyPunchForceToCharacter(const FVector FromLocation, const float InAttackForce) const
{
	const FVector TargetLocation = GetActorLocation();
	const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation, TargetLocation);

	GetCharacterMovement()->Launch(FVector(
		Direction.X * AttackForce,
		Direction.Y * AttackForce,
		abs(Direction.Z + 1) * AttackForce));
}

void AMBSBaseCharacter::OnFootstepNative()
{
	if (bFootstepGate)
	{
		bFootstepGate = !bFootstepGate;
		if (OnFootstepTaken.IsBound())
		{
			OnFootstepTaken.Broadcast();
		}
		GetWorldTimerManager().SetTimer(FootstepTimer, [this]() { bFootstepGate = true; }, 0.3f, false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMBSBaseCharacter::OnDeathNative()
{
	if (OnCharacterDeath.IsBound())
	{
		OnCharacterDeath.Broadcast();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMBSBaseCharacter::OnAttackEndNative()
{
	if (OnAttackTimerEnd.IsBound())
	{
		OnAttackTimerEnd.Broadcast();
	}
}

float AMBSBaseCharacter::GetHealth() const
{
	if (!Attributes)
	{
		return 1.f;
	}
	return Attributes->GetHealth();
}

float AMBSBaseCharacter::GetMaxHealth() const
{
	return Attributes->GetMaxHealth();
}

void AMBSBaseCharacter::Animate(float DeltaTime, FVector OldLocation, const FVector OldVelocity)
{
	if (!bAttackGate)
	{
		return;
	}
	TOptional<FMinimalViewInfo> ViewInfo;
	if (!IsPlayerControlled())
	{
		const UWorld* World = GetWorld();
		if (World)
		{
			const APlayerController* PlayerController = World->GetFirstPlayerController();
			if (PlayerController)
			{
				ACharacter* Character = PlayerController->GetCharacter();
				if (Character)
				{
					Character->CalcCamera(DeltaTime, ViewInfo.Emplace());
				}
			}
		}
	}

	SetCurrentAnimationDirection(OldVelocity, ViewInfo);

	if (OldVelocity.Size() > 0.0f || GetCharacterMovement()->IsFalling())
	{
		switch (CurrentAnimationDirection)
		{
		case EMBSAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUp);
			break;
		case EMBSAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDown);
			break;
		case EMBSAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkLeft);
			break;
		case EMBSAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkRight);
			break;
		case EMBSAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpLeft);
			break;
		case EMBSAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpRight);
			break;
		case EMBSAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownLeft);
			break;
		case EMBSAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownRight);
			break;
		default:
			break;
		}

		if (!GetCharacterMovement()->IsFalling())
		{
			if (OnFootstepTakenNative.IsBound())
			{
				OnFootstepTakenNative.Broadcast();
			}
		}
	}
	else
	{
		switch (CurrentAnimationDirection)
		{
		case EMBSAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUp);
			break;
		case EMBSAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDown);
			break;
		case EMBSAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleLeft);
			break;
		case EMBSAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleRight);
			break;
		case EMBSAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpLeft);
			break;
		case EMBSAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpRight);
			break;
		case EMBSAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDownLeft);
			break;
		case EMBSAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDownRight);
			break;
		default:
			break;
		}
	}

	if (GetCharacterMovement()->IsFalling())
	{
		GetSprite()->SetPlayRate(0.f);
		GetSprite()->SetPlaybackPositionInFrames(0, true);
	}
	else
	{
		GetSprite()->SetPlayRate(1.f);
	}
}

void AMBSBaseCharacter::SetCurrentAnimationDirection(const FVector& Velocity, TOptional<FMinimalViewInfo> ViewInfo)
{
	FVector Forward;
	FVector Right;
	if (ViewInfo.IsSet())
	{
		Forward = UKismetMathLibrary::GetForwardVector(ViewInfo.GetValue().Rotation);
		Right = UKismetMathLibrary::GetRightVector(ViewInfo.GetValue().Rotation);
	}
	else
	{
		Forward = GetActorForwardVector().GetSafeNormal();
		Right = GetActorRightVector().GetSafeNormal();
	}

	const float ForwardSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Forward) * 100) / 100;
	const float RightSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Right) * 100) / 100;

	bIsMoving = ForwardSpeed != 0.0f || RightSpeed != 0.0f;

	if (bIsMoving && !GetCharacterMovement()->IsFalling())
	{
		if (ForwardSpeed > 0.0f && abs(RightSpeed) < 0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::Up;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::UpRight;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::UpLeft;
		}
		else if (ForwardSpeed < 0.5f && abs(RightSpeed) <= 0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::Down;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::DownRight;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::DownLeft;
		}
		else if (abs(ForwardSpeed) < 0.5f && RightSpeed > 0.0f)
		{
			CurrentAnimationDirection = EMBSAnimationDirection::Right;
		}
		else
		{
			CurrentAnimationDirection = EMBSAnimationDirection::Left;
		}
	}
}

void AMBSBaseCharacter::PlayPunchAnimation_Implementation(const float TimerValue)
{
	FVector NewLocation{GetSprite()->GetRelativeLocation()};

	if (bAttackGate)
	{
		switch (CurrentAnimationDirection)
		{
		case EMBSAnimationDirection::Down:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDown);
			NewLocation.X -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::Up:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUp);
			NewLocation.X += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::Right:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchRight);
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::Left:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchLeft);
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownRight);
			NewLocation.X -= 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownLeft);
			NewLocation.X -= 25.f;
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUpRight);
			NewLocation.X += 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EMBSAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUpLeft);
			NewLocation.X += 25.f;
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		default:
			break;
		}

		bAttackGate = false;
		GetWorldTimerManager().SetTimer(AttackTimer, [this]()
		 {
			bAttackGate = true;
			GetSprite()->SetRelativeLocation(FVector::ZeroVector);
			if (OnAttackTimerEndNative.IsBound())
			{
				OnAttackTimerEndNative.Broadcast();
			}
		 }, TimerValue, false);
	}
}

UAbilitySystemComponent* AMBSBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMBSBaseCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UMBSGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}

		// Now apply passives
		for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}

void AMBSBaseCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, AMBSBaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void AMBSBaseCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
		if (GetHealth() <= 0)
		{
			UE_LOG(LogMonsterBattleSystem, Warning, TEXT("Adding DeadTag"))
			AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		}
	}
}
