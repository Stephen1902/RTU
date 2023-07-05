// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterComponent.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaterChanged, float, WaterValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UWaterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWaterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Water")
	double MaxWater;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentWater)
	double CurrentWater;

	UPROPERTY(Replicated)
	double WaterDrainMultiplier;

	// Drain rate for Water when the actor using this component is doing nothing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " Water ")
	double WaterDrainRate;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWaterChanged OnWaterChanged;

	void AmendWaterValue(double WaterAmountChange);
	void AmendWaterMultiplier(double WaterMultiplierChange);

protected:
	UFUNCTION()
	void OnRep_CurrentWater();

	void DrainWater(float DeltaTime);	
private:
	// Check integer for if the HUD needs updating
	int32 WaterFlooredToInt;

	// Update the HUD if needed
	void CheckForHUDUpdate();
};
