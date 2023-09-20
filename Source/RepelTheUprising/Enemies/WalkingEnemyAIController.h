// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WalkingEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API AWalkingEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	AWalkingEnemyAIController();

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behaviour")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	// Time in seconds before a disabled enemy will return to an active state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeReset;

	// Time in seconds before an alerted enemy returns to a normal state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeAlertOff;

	// Time in seconds before an enemy chasing will stop and return to an Alert state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeCallOffChase;

	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;
		
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* FoundActor, FAIStimulus Stimulus);

	UFUNCTION()
	void TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	bool bIsChasingTarget = false;

	// The current player character that this enemy has spotted
	UPROPERTY()
	TObjectPtr<class ARepelTheUprisingCharacter> RTUPlayerCharacter;

	// Character that this controller is controlling
	UPROPERTY()
	TObjectPtr<AWalkingEnemyBase> ControlledCharacter;
};
