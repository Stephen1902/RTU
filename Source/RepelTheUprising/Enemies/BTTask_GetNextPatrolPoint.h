// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UBTTask_GetNextPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_GetNextPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
