// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FoodComponent.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoodChanged, float, FoodValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UFoodComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFoodComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Food")
	double MaxFood;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentFood)
	double CurrentFood;

	UPROPERTY(Replicated)
	double FoodDrainMultiplier;

	// Drain rate for food when the actor using this component is doing nothing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " Food ")
	double FoodDrainRate;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFoodChanged OnFoodChanged;

	void AmendFoodValue(double FoodAmountChange);
	void AmendFoodMultiplier(double FoodMultiplierChange);

protected:
	UFUNCTION()
	void OnRep_CurrentFood();

	void DrainFood(float DeltaTime);	
private:
	// Check integer for if the HUD needs updating
	int32 FoodFlooredToInt;

	// Update the HUD if needed
	void CheckForHUDUpdate();
};
