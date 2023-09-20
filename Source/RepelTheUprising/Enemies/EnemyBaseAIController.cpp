// Copyright 2023 DME Games


#include "EnemyBaseAIController.h"
#include "EnemyBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

AEnemyBaseAIController::AEnemyBaseAIController()
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
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBaseAIController::TargetPerceptionUpdated);
	AIPerceptionComp->OnTargetPerceptionInfoUpdated.AddDynamic(this, &AEnemyBaseAIController::TargetPerceptionInfoUpdated);
}

void AEnemyBaseAIController::TargetPerceptionUpdated(AActor* FoundActor, FAIStimulus Stimulus)
{
	if (!Stimulus.WasSuccessfullySensed())
	{
		bIsChasingTarget = false;
		UE_LOG(LogTemp, Warning, TEXT("I lost whatever I saw."));
	}
	else
	{
		// Check there is a valid pawn being controlled by this controller
		if (AEnemyBase* ControlledPawn = Cast<AEnemyBase>(GetPawn()))
		{
			UE_LOG(LogTemp, Warning, TEXT("I see something."));

			bIsChasingTarget = true;
			ControlledPawn->SetNewStatus(EStaticEnemyStatus::EES_Chasing);
		}
	}

}

void AEnemyBaseAIController::TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (bIsChasingTarget)
	{
		if (UpdateInfo.Stimulus.WasSuccessfullySensed())
		{
			MoveToTargetLocation(UpdateInfo.Stimulus.StimulusLocation);
		}
		else
		{
			bIsChasingTarget = false;
		}
	}
}

void AEnemyBaseAIController::MoveToTargetLocation(const FVector& TargetLocation)
{
	// Only move and chase if there is a valid player character
	if (TargetLocation != FVector::ZeroVector)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
	}
}

