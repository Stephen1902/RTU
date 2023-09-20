// Copyright 2023 DME Games


#include "WalkingEnemyAIController.h"

#include "WalkingEnemyBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RepelTheUprising/Player/RepelTheUprisingCharacter.h"

AWalkingEnemyAIController::AWalkingEnemyAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Info"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1500.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Info"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	// Called when an enemy's perception is first triggered by a new stimuli
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AWalkingEnemyAIController::TargetPerceptionUpdated);
	// Called when an enemy's perception is updated, whether by losing sight or sound or updating a seen / heard actor
	AIPerceptionComp->OnTargetPerceptionInfoUpdated.AddDynamic(this, &AWalkingEnemyAIController::TargetPerceptionInfoUpdated);

	// Set default variables
	TimeBeforeReset = 30.;
	TimeBeforeAlertOff = 30.;
	TimeBeforeCallOffChase = 30.;
}

void AWalkingEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Check for a valid Behaviour Tree, run it if there is one
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}

	// Get the character being controlled
	ControlledCharacter = Cast<AWalkingEnemyBase>(GetCharacter());
}

void AWalkingEnemyAIController::TargetPerceptionUpdated(AActor* FoundActor, FAIStimulus Stimulus)
{
	// Check there is a valid pawn being controlled by this controller
	if (ControlledCharacter == nullptr)	{ return; }

	// Check if 
	if (!Stimulus.WasSuccessfullySensed())
	{
		bIsChasingTarget = false;
		ControlledCharacter->SetNewStatus(EWalkingEnemyStatus::EES_Alert);
	}
	else
	{
		// Check what was spotted was a player character
			RTUPlayerCharacter = Cast<ARepelTheUprisingCharacter>(FoundActor);
			if (RTUPlayerCharacter != nullptr)
			{
				bIsChasingTarget = true;
				ControlledCharacter->SetNewStatus(EWalkingEnemyStatus::EES_Chasing);
		
				GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), RTUPlayerCharacter->GetActorLocation());
				const FVector ValueAsSet = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
				UE_LOG(LogTemp, Warning, TEXT("Value set to: %s"), *ValueAsSet.ToString());
				UE_LOG(LogTemp, Warning, TEXT("I saw a player in location %s"), *ValueAsSet.ToString());
			}
	}

}

void AWalkingEnemyAIController::TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	// Check there is a valid pawn being controlled by this controller
	if (ControlledCharacter == nullptr)	{ return; }

	// Check if the enemy is already chasing something
	if (bIsChasingTarget)
	{
		// Check if this character still has an enemy in range
		if (UpdateInfo.Stimulus.WasSuccessfullySensed())
		{
			// Check if what has just been spotted is what this enemy was already chasing
			if (RTUPlayerCharacter != nullptr)
			{
				GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), RTUPlayerCharacter->GetActorLocation());
			}
			//MoveToTargetLocation(UpdateInfo.Stimulus.StimulusLocation);
		}
		else
		{
			// Update the last known location of the player being chased
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation")));
			// Clear the player's location BB Key and the player reference
			GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
			RTUPlayerCharacter = nullptr;
			
			bIsChasingTarget = false;
		}
	}
}
