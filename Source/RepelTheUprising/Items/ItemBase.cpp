// Copyright 2023 DME Games


#include "ItemBase.h"
#include "Net/UnrealNetwork.h"

void UItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemBase, CurrentHealth);
}

void UItemBase::OnRep_CurrentHealth()
{
	// Replicate the current health to the clients, if needed
}
