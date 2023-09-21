// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_WaitBlackboardTime.h"
#include "BTTask_PatrolPointWait.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UBTTask_PatrolPointWait : public UBTTask_WaitBlackboardTime
{
	GENERATED_BODY()

	UBTTask_PatrolPointWait();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
