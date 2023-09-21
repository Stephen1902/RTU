// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_StartSearchTimer.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UBT_StartSearchTimer : public UBTTaskNode
{
	GENERATED_BODY()

	UBT_StartSearchTimer();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
