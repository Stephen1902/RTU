// Copyright 2023 DME Games


#include "StaminaComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Stamina should be replicated across the game
	SetIsReplicatedByDefault(true);
		
	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaRegenerationRate = 10.0f;
	TimeBeforeRegeneration = 2.0f;
	TimeSinceStaminaUsed = 0.f;
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check if stamina can regenerate
	if (TimeSinceStaminaUsed += DeltaTime > TimeBeforeRegeneration && CurrentStamina < MaxStamina)
	{
		// Only run on the server
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenerationRate * DeltaTime, 0.0f, MaxStamina);
		}
	}
}

void UStaminaComponent::ModifyStamina(float Value)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + Value, 0.0f, MaxStamina);
	}
}

void UStaminaComponent::OnRep_CurrentStamina()
{
	// Handle stamina replication updates on clients if needed
	OnStaminaChanged.Broadcast(this, CurrentStamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
}

