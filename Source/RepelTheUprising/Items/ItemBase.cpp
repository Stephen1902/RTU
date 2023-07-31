// Copyright 2023 DME Games


#include "ItemBase.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "Item"

void UItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemBase, CurrentHealth);
	DOREPLIFETIME(UItemBase, CurrentQuantity);
}

bool UItemBase::IsSupportedForNetworking() const
{
	return true;
}

UItemBase::UItemBase()
{
	// Default settings for items go here
	
	ItemName = LOCTEXT("ItemName", "Item");
	ItemDescription = LOCTEXT("ItemDescription", "Description");
	ItemUseText = LOCTEXT("ItemUse", "Use");
	ItemBaseCost = 0.f;
	ItemWeight = 0.f;
	bCanBeStacked = false;
	MaxStackSize = 1;
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	ItemLevel = 1;
	CurrentQuantity = 1;
	bShouldShowInInventory = true;

	RepKey = 0;
	
}

void UItemBase::OnRep_CurrentHealth()
{
	// Replicate the current health to the clients, if needed
	OnItemModified.Broadcast();
}

void UItemBase::OnRep_CurrentQty()
{
	// Replicate the current quantity to clients, if needed
	OnItemModified.Broadcast();
}

void UItemBase::Use(TObjectPtr<ARepelTheUprisingCharacter> Character)
{
}

void UItemBase::AddedToInventory(TObjectPtr<UInventoryComponent> Inventory)
{
}

void UItemBase::SetHealth(const int32 NewHealth)
{
	// Only update the health if it's different to the existing health
	if (NewHealth != CurrentHealth)
	{
		// Update the current health, clamping it between 0 and it's maximum health
		CurrentHealth = FMath::Clamp(NewHealth, 0, MaxHealth);
		MarkDirtyForReplication();
	}
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	// Only have things changed if the New Quantity is different to the Current Quantity
	if (NewQuantity != CurrentQuantity)
	{
		// Update the current quantity, making sure it is never less than zero or higher than the max stack size / 1 if it can't be stacked
		CurrentQuantity = FMath::Clamp(NewQuantity, 0, bCanBeStacked ?  MaxStackSize : 1);
		MarkDirtyForReplication();
	}
}

void UItemBase::ShouldDisplayInInventory(TEnumAsByte<EDisplayInInventory>& Branches)
{
	if (bShouldShowInInventory)
	{
		Branches = DisplayYes;
	}
	else
	{
		Branches = DisplayNo;
	}
}

void UItemBase::IsQuantityGreaterThanZero(TEnumAsByte<EDisplayInInventory>& Branches)
{
	if (CurrentQuantity != 0)
	{
		Branches = DisplayYes;
	}
	else
	{
		Branches = DisplayNo;
	}
}

FText UItemBase::GetQuantityAsText() const
{
	const FString QuantityAsString = FString::FromInt(CurrentQuantity);
	return FText::FromString(QuantityAsString);
}

/*
bool UItemBase::ShouldShowInInventory() const
{
	return bShouldShowInInventory;
}
*/
void UItemBase::MarkDirtyForReplication()
{
}

#undef LOCTEXT_NAMESPACE
