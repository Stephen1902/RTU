// Copyright 2023 DME Games

#include "WaterComponent.h"
#include "Net/UnrealNetwork.h"

UWaterComponent::UWaterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxWater = 100.0;
	CurrentWater = MaxWater;
	WaterFlooredToInt = MaxWater;

	WaterDrainRate = 5.0;  // For testing purposes only, TODO find a better rate
	WaterDrainMultiplier = 1.0;

	SetIsReplicatedByDefault(true);
}

void UWaterComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UWaterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		DrainWater(DeltaTime);
	}
}

void UWaterComponent::DrainWater(float DeltaTime)
{
	const float DrainThisDelta = (WaterDrainRate * WaterDrainMultiplier) * DeltaTime;
	CurrentWater = FMath::Clamp(CurrentWater - DrainThisDelta, 0.0f, MaxWater);
	CheckForHUDUpdate();
}

void UWaterComponent::CheckForHUDUpdate()
{
	// Only update the Water HUD if it has changed by more than an integer
	const int32 CurrentWaterFloored = FMath::Floor(CurrentWater);
	if (WaterFlooredToInt != CurrentWaterFloored)
	{
		// Stamina needs to be set as a percentage for the HUD
		const float WaterAsPercentage = 1.0f - ((MaxWater - CurrentWater) / MaxWater);
		OnWaterChanged.Broadcast(WaterAsPercentage);
		
		// Update the floored value
		WaterFlooredToInt = CurrentWaterFloored;
	}
}

void UWaterComponent::OnRep_CurrentWater()
{
	// Handle stamina replication updates on clients if needed
	CheckForHUDUpdate();
}

void UWaterComponent::AmendWaterValue(const double WaterAmountChange)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentWater += WaterAmountChange;
	}
}

void UWaterComponent::AmendWaterMultiplier(const double WaterMultiplierChange)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		WaterDrainMultiplier += WaterMultiplierChange;
	}
}

void UWaterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWaterComponent, CurrentWater);
	DOREPLIFETIME(UWaterComponent, WaterDrainMultiplier);
}
