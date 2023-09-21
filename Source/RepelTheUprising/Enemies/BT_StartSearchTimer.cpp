// Copyright 2023 DME Games


#include "BT_StartSearchTimer.h"

#include "WalkingEnemyAIController.h"

UBT_StartSearchTimer::UBT_StartSearchTimer()
{
	NodeName = TEXT("Start Search Timer");
}

EBTNodeResult::Type UBT_StartSearchTimer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (AWalkingEnemyAIController* EnemyAIController = Cast<AWalkingEnemyAIController>(OwnerComp.GetAIOwner()))
	{
		

		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
