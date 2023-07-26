// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Item_FoodLiquidHealth.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UItem_FoodLiquidHealth : public UItemBase
{
	GENERATED_BODY()

protected:
	UItem_FoodLiquidHealth();
	
	UPROPERTY(EditAnywhere, Category = "FIH Set Up")
	double AmendFoodValue;

	UPROPERTY(EditAnywhere, Category = "FIH Set Up")
	double AmendWaterValue;

	UPROPERTY(EditAnywhere, Category = "FIH Set Up")
	double AmendHealthValue;
};
