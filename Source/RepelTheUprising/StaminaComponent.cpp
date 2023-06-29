// Copyright 2023 DME Games


#include "StaminaComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaDrainRate = 5.0f;
	StaminaRegenerationRate = 10.0f;

	SetIsReplicatedByDefault(true);
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	const float StaminaAsPercentage = 1.0f - ((MaxStamina - CurrentStamina) / MaxStamina);
	OnStaminaChanged.Broadcast(1.0f);
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
}

void UStaminaComponent::DrainStamina(float DeltaTime)
{
	const float DrainThisDelta = StaminaDrainRate * DeltaTime;
	CurrentStamina = FMath::Clamp(CurrentStamina - DrainThisDelta, 0.0f, MaxStamina);
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
	if (CurrentStamina >= MaxStamina)
	{
		return;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenerationRate * DeltaTime, 0.0f, MaxStamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
}

