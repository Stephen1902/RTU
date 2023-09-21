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

public:
	// Called by the behaviour tree to start the call of the alert status
	void BeginCallOffAlert();
	// Called when the timer for the alert is complete or when the player is spotted again
	void CallOffAlert();
	// Location of the current Patrol Point
	void GetPatrolPointLocation();
	
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
	
	// Time to wait in between moving between patrol points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitTimeBetweenPatrolPoints;
	
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
	TObjectPtr<class AWalkingEnemyBase> ControlledCharacter;

	// Timer Handles
	FTimerHandle CallOffAlertTimer;

	// Location of the current patrol point for this enemy
	int32 CurrentPatrolPoint;

};
