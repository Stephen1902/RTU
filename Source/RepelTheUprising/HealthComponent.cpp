// Copyright 2023 DME Games


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Health should be replicated across the game
	SetIsReplicatedByDefault(true);

	DefaultHealth = 100.f;
	HealthRegenPerSecond = 0.1f;
	TimeBeforeHealing = 2.0f;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set the function to handle damage only if this is the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (AActor* MyOwner = GetOwner())
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::UHealthComponent::HandleDamage);
		}
	}

	CurrentHealth = DefaultHealth;
	TimeSinceLastDamage = TimeBeforeHealing;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TryToHeal(DeltaTime);
}

void UHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Check that damage has actually taken place
	if (Damage <= 0.f)
	{
		return;
	}

	// Update the current health ensuring it does not go below zero or above max / default health
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, DefaultHealth);
	TimeSinceLastDamage = 0.f;
	OnHealthChanged.Broadcast(this, CurrentHealth, DamageType, InstigatedBy, DamageCauser);
}

void UHealthComponent::TryToHeal(float DeltaTime)
{
	// Only allow this function to run on the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		TimeSinceLastDamage += DeltaTime;
		if (TimeSinceLastDamage >= TimeBeforeHealing)
		{
			CurrentHealth += FMath::Clamp(CurrentHealth + (HealthRegenPerSecond * DeltaTime), 0.f, DefaultHealth);
			OnHealthChanged.Broadcast(this, CurrentHealth, nullptr, nullptr, nullptr);
		}
	}
}
