// © Argo. All rights reserved.

#include "Core/AI/MBSAIController.h"

#include "Actors/MBSBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMBSAIController::AMBSAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));

	SenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("SenseConfigSight");

	SenseConfig_Sight.Get()->SightRadius = 1500.f;
	SenseConfig_Sight.Get()->LoseSightRadius = 2000.f;
	SenseConfig_Sight.Get()->PeripheralVisionAngleDegrees = 360.f;
	SenseConfig_Sight.Get()->SetMaxAge(5.f);
	SenseConfig_Sight.Get()->AutoSuccessRangeFromLastSeenLocation = 900.f;
	SenseConfig_Sight.Get()->DetectionByAffiliation.bDetectEnemies = true;
	SenseConfig_Sight.Get()->DetectionByAffiliation.bDetectFriendlies = true;
	SenseConfig_Sight.Get()->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent.Get()->ConfigureSense(*SenseConfig_Sight);
	AIPerceptionComponent.Get()->SetDominantSense(*SenseConfig_Sight->GetSenseImplementation());
}

TObjectPtr<UBehaviorTreeComponent> AMBSAIController::GetBehaviorTreeComponent() const
{
	return BehaviorTreeComponent;
}

void AMBSAIController::BeginPlay()
{
	Super::BeginPlay();

	AIPerceptionComponent.Get()->OnTargetPerceptionUpdated.AddDynamic(this, &AMBSAIController::OnActorSensed);

	if (IsValid(BlackboardData.Get()))
	{
		// Cannot use TObjectPtr for the BlackboardComponent due to this function
		UseBlackboard(BlackboardData.Get(), BlackboardComponent);
	}

	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}
}

void AMBSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMBSAIController::OnActorSensed(AActor* Actor, FAIStimulus Stimulus)
{
	const TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(Actor, Stimulus);

	if (Stimulus.WasSuccessfullySensed())
	{
		// Only chase the player, don't chase other enemies
		if (SenseClass == UAISense_Sight::StaticClass() && Actor->GetClass() == PlayerClass)
		{
			BlackboardComponent->SetValueAsBool("PlayerSpotted", true);
			BlackboardComponent->SetValueAsVector("TargetLocation", Stimulus.StimulusLocation);
			BlackboardComponent->SetValueAsObject("TargetActor", Actor);
		}
	}
	else
	{
		if (SenseClass == UAISense_Sight::StaticClass())
		{
			BlackboardComponent->SetValueAsBool("PlayerSpotted", false);
		}
	}
}
