// © Argo. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "MBSAIController.generated.h"

class AMBSBaseCharacter;
class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * Custom AIController for Hopper game
 */
UCLASS()
class MONSTERBATTLESYSTEM_API AMBSAIController : public AAIController
{
	GENERATED_BODY()
public:
	AMBSAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TSubclassOf<AMBSBaseCharacter> PlayerClass;
	
	TObjectPtr<UAISenseConfig_Sight> SenseConfig_Sight;
	TObjectPtr<UBlackboardData> BlackboardData;
	
private:
	UFUNCTION()
	void OnActorSensed(AActor* Actor, FAIStimulus Stimulus);

public:
	TObjectPtr<UBehaviorTreeComponent> GetBehaviorTreeComponent() const;
};
