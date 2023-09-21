// Copyright 2023 DME Games


#include "BTTask_GetNextPatrolPoint.h"

#include "WalkingEnemyAIController.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
	NodeName = TEXT("Get Next Patrol Point");
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (AWalkingEnemyAIController* EnemyAIController = Cast<AWalkingEnemyAIController>(OwnerComp.GetAIOwner()))
	{
		EnemyAIController->GetPatrolPointLocation();
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
