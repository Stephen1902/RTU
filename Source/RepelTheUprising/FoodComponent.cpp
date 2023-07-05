// Copyright 2023 DME Games

#include "FoodComponent.h"
#include "Net/UnrealNetwork.h"

UFoodComponent::UFoodComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxFood = 100.0;
	CurrentFood = MaxFood;
FoodFlooredToInt = MaxFood;

	FoodDrainRate = 5.0;  // For testing purposes only, TODO find a better rate
	FoodDrainMultiplier = 1.0;

	SetIsReplicatedByDefault(true);
}

void UFoodComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UFoodComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		DrainFood(DeltaTime);
	}
}

void UFoodComponent::DrainFood(float DeltaTime)
{
	const float DrainThisDelta = (FoodDrainRate * FoodDrainMultiplier) * DeltaTime;
	CurrentFood = FMath::Clamp(CurrentFood - DrainThisDelta, 0.0f, MaxFood);
	CheckForHUDUpdate();
}

void UFoodComponent::CheckForHUDUpdate()
{
	// Only update the food HUD if it has changed by more than an integer
	const int32 CurrentFoodFloored = FMath::Floor(CurrentFood);
	if (FoodFlooredToInt != CurrentFoodFloored)
	{
		// Stamina needs to be set as a percentage for the HUD
		const float FoodAsPercentage = 1.0f - ((MaxFood - CurrentFood) / MaxFood);
		OnFoodChanged.Broadcast(FoodAsPercentage);
		
		// Update the floored value
		FoodFlooredToInt = CurrentFoodFloored;
	}
}

void UFoodComponent::OnRep_CurrentFood()
{
	// Handle stamina replication updates on clients if needed
	CheckForHUDUpdate();
}

void UFoodComponent::AmendFoodValue(const double FoodAmountChange)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentFood += FoodAmountChange;
	}
}

void UFoodComponent::AmendFoodMultiplier(const double FoodMultiplierChange)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FoodDrainMultiplier += FoodMultiplierChange;
	}
}

void UFoodComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFoodComponent, CurrentFood);
	DOREPLIFETIME(UFoodComponent, FoodDrainMultiplier);
}
