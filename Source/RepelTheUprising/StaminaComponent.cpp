// Copyright 2023 DME Games

#include "StaminaComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxStamina = 100.0;
	CurrentStamina = MaxStamina;
	DefaultStaminaDrainRate = 5.0;
	CurrentStaminaDrainRate = DefaultStaminaDrainRate;
	DefaultStaminaRegenerationRate = 10.0;
	CurrentStaminaRegenerationRate = DefaultStaminaRegenerationRate;
	TimeBeforeStaminaRegen = 2.0;
	TimeSinceStaminaUsed = 0.0;
	StaminaMultiplier = 1.0;

	SetIsReplicatedByDefault(true);
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// Make sure the stamina has actually been changed
		const float StaminaBeforeCheck = CurrentStamina;
		if (bStaminaIsDraining)
		{
			DrainStamina(DeltaTime);
		}
		else
		{
			RegenerateStamina(DeltaTime);
		}
		
		if (CurrentStamina != StaminaBeforeCheck)
		{
			const float StaminaAsPercentage = 1.0f - ((MaxStamina - CurrentStamina) / MaxStamina);
			OnStaminaChanged.Broadcast(StaminaAsPercentage);
		}
	}
}

void UStaminaComponent::SetStaminaShouldDrain(bool bNewStateIn)
{
	bStaminaIsDraining = bNewStateIn;
	if (!bStaminaIsDraining)
	{
		TimeSinceStaminaUsed = 0.0;
	}
}

void UStaminaComponent::AdjustStaminaModifier(double StaminaAdjustmentIn)
{
}

void UStaminaComponent::DrainStamina(float DeltaTime)
{
	const float DrainThisDelta = CurrentStaminaDrainRate * DeltaTime;
	CurrentStamina = FMath::Clamp(CurrentStamina - DrainThisDelta, 0.0, MaxStamina);
}

void UStaminaComponent::OnRep_CurrentStamina()
{
	// Handle stamina replication updates on clients if needed
	
	// Stamina needs to be set as a percentage for the HUD
	const float StaminaAsPercentage = 1.0f - ((MaxStamina - CurrentStamina) / MaxStamina);
	OnStaminaChanged.Broadcast(StaminaAsPercentage);
}

void UStaminaComponent::RegenerateStamina(float DeltaTime)
{
	TimeSinceStaminaUsed += DeltaTime;

	// Only allow the stamina to regenerate if the actor isn't already at maximum stamina or if enough time has passed since they used stamina
	if (CurrentStamina >= MaxStamina || TimeSinceStaminaUsed < TimeBeforeStaminaRegen)
	{
		return;
	}

	// Add the amount of regeneration, allowing for the frame rate
	CurrentStamina = FMath::Clamp(CurrentStamina + (CurrentStaminaRegenerationRate * DeltaTime), 0.0, MaxStamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
}

