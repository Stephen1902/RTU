// Copyright 2023 DME Games


#include "WalkingEnemyBase.h"

#include "WalkingEnemyAIController.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWalkingEnemyBase::AWalkingEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The server and all clients need to be able to see the enemy
	SetReplicates(true);
	ACharacter::SetReplicateMovement(true);

	// Set the mesh location and rotation for what are standard sized characters
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	// Set up the AI information for all subclasses
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//AIControllerClass = AWalkingEnemyAIController::StaticClass();

	// Set default variables
	EnemyStatus = EWalkingEnemyStatus::EES_Normal;
	CurrentPatrolPoint = 0;
}

void AWalkingEnemyBase::SetNewStatus(EWalkingEnemyStatus NewStatusIn)
{
	EnemyStatus = NewStatusIn;
	
	OnStatusChanged();
}

// Called when the game starts or when spawned
void AWalkingEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWalkingEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWalkingEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AWalkingEnemyBase::GetFirstPatrolPoint()
{
	if (PatrolPoints.Num() > 0)
	{
		return PatrolPoints[0]->GetActorLocation();
	}
	return FVector(0.f);
}

FVector AWalkingEnemyBase::GetNextPatrolPoint()
{
	if (PatrolPoints.Num() > 0)
	{
		const int32 TempPatrolPoint = CurrentPatrolPoint + 1;
		CurrentPatrolPoint = TempPatrolPoint % PatrolPoints.Num();
		UE_LOG(LogTemp, Warning, TEXT("CurrentPatrolPoint set to: %i"), CurrentPatrolPoint);
		return PatrolPoints[CurrentPatrolPoint]->GetActorLocation();
	
	}
	return FVector(0.f);
}

void AWalkingEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWalkingEnemyBase, CurrentMovementSpeed);
}

void AWalkingEnemyBase::OnStatusChanged()
{
	switch (EnemyStatus)
	{
	case EWalkingEnemyStatus::EES_Normal:
		break;
	case EWalkingEnemyStatus::EES_Chasing:
		break;
	case EWalkingEnemyStatus::EES_Alert:
		break;
	case EWalkingEnemyStatus::EES_Combat:
		break;
	case EWalkingEnemyStatus::EES_Disabled:
		break;
	
	default: break;
	}
}

