// Copyright 2023 DME Games


#include "BTTask_PatrolPointWait.h"

UBTTask_PatrolPointWait::UBTTask_PatrolPointWait()
{
	NodeName = TEXT("Wait Before Moving");
}

EBTNodeResult::Type UBTTask_PatrolPointWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
