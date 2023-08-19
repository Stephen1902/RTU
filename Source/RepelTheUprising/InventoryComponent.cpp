// Copyright 2023 DME Games


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Items/ItemBase.h"

#define LOCTEXT_NAMESPACE "Inventory"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


void UInventoryComponent::ClientRefreshInventory_Implementation()
{
	// Tell anything listening that the inventory has been updated
	OnInventoryUpdated.Broadcast();
}

FItemAddResult UInventoryComponent::TryToAddItem(UItemBase* ItemToAdd)
{
	return TryAddItem_Internal(ItemToAdd);
}

FItemAddResult UInventoryComponent::TryToAddItemFromClass(TSubclassOf<UItemBase> ItemClass, const int32 Quantity)
{
	UItemBase* NewItem = NewObject<UItemBase>(GetOwner(), ItemClass);
	NewItem->SetQuantity(Quantity);
	return TryAddItem_Internal(NewItem);
}

bool UInventoryComponent::RemoveItem(UItemBase* ItemToRemove)
{
	// Can only be run by the server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// Check for a valid item
		if (ItemToRemove)
		{
			Items.RemoveSingle(ItemToRemove);

			// Adjust the replicated item key			
			ReplicatedItemsKey++;

			// Function was successful
			return true;
		}
	}

	// Function had no item to remove or was called by a client
	return false;
}

bool UInventoryComponent::HasItem(TSubclassOf<UItemBase> ItemClass, const int32 Quantity)
{
	// Check the array to see if the item given to the function is in it
	if (TObjectPtr<UItemBase> ItemToFind = FindItemByClass(ItemClass))
	{
		// Return this item only if the quantity in the inventory is greater or equal to the quantity needed
		return ItemToFind->GetQuantityAsInt() >= Quantity;
	}
	// If the item is not found or there is not the full quantity, return false
	return false;
}

UItemBase* UInventoryComponent::FindItem(UItemBase* ItemIn) const
{
	// Check the ItemIn is valid
	if (ItemIn)
	{
		// Loop through the array of items in this inventory
		for (const TObjectPtr<UItemBase>& InvItem : Items)
		{
			// Check if the classes match and return the item if it does
			if (InvItem && InvItem->GetClass() == ItemIn->GetClass())
			{
				return InvItem;
			}
		}
	}
	// If no match was found, return nothing
	return nullptr;
}

UItemBase* UInventoryComponent::FindItemByClass(TSubclassOf<UItemBase> ItemClass) const
{
	// Loop through the array of items in this inventory
	for (const TObjectPtr<UItemBase>& InvItem : Items)
	{
		// Check if the item is valid and if it matches the ItemClass given to the function 
		if (InvItem && InvItem->GetClass() == ItemClass)
		{
			return InvItem;
		}
	}
	// If the array is completed without a match, the inventory does not have this item
	return nullptr;
}

TArray<UItemBase*> UInventoryComponent::FindItemsByClass(TSubclassOf<UItemBase> ItemClass) const
{
	// Create a blank array of items
	TArray<UItemBase*> ItemsOfClass;

	// Loop through the array of items in this inventory
	for (const TObjectPtr<UItemBase>& InvItem : Items)
	{
		// Check whether the class of this item matches the class given to the function, adding it to the new array if it does
		if (InvItem && InvItem->GetClass()->IsChildOf(ItemClass))
		{
			ItemsOfClass.Add(InvItem);
		}
	}

	return ItemsOfClass;
}

double UInventoryComponent::GetCurrentWeight() const
{
	double CurrentWeight = 0.;

	// Loop through the array of items in this inventory
	for (const TObjectPtr<UItemBase>& InvItem : Items)
	{
		// Check the item is valid
		if (InvItem)
		{
			CurrentWeight += InvItem->GetStackWeight();
		}
	}

	return CurrentWeight;
}

void UInventoryComponent::SetWeightCapacity(const double NewWeightCapacity)
{
	WeightCapacity = NewWeightCapacity;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	ItemCapacity = NewCapacity;
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::ConsumeItem(UItemBase* ItemIn)
{
	// Check there is a valid item being entered
	if (ItemIn)
	{
		// Call the overloaded function, giving the quantity as the full amount in the inventory
		ConsumeItem(ItemIn, ItemIn->GetQuantityAsInt());
	}
	return 0;
}

int32 UInventoryComponent::ConsumeItem(UItemBase* ItemIn, const int32 QuantityIn)
{
	// Can only amend quantities if it is the server and if the ItemIn is valid
	if (GetOwner() && GetOwner()->HasAuthority() && ItemIn)
	{
		// Work out how much of the item, ensuring more cannot be used than the inventory has
		const int32 RemoveQuantity = FMath::Min(QuantityIn, ItemIn->GetQuantityAsInt());

		// There should never be less than zero of an item, halt if this has happened
		ensure(!(ItemIn->GetQuantityAsInt() - RemoveQuantity < 0));

		// Set the new quantity, having taken away the RemoveQuantity
		ItemIn->SetQuantity(ItemIn->GetQuantityAsInt() - RemoveQuantity);

		// If the quantity is now zero, remove it from the inventory
		if (ItemIn->GetQuantityAsInt() <= 0)
		{
			RemoveItem(ItemIn);
		}
		else
		{
			ClientRefreshInventory();
		}

		// Return this function with the amount removed
		return RemoveQuantity;
	}

	// Nothing was removed.  Return this.
	return 0;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bChangedSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		for (auto& Item : Items)
		{
			if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
			{
				bChangedSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}
	}

	return bChangedSomething;
}

void UInventoryComponent::OnRep_Items()
{
	// Tell anything listening that the inventory has been updated
	OnInventoryUpdated.Broadcast();
}

FItemAddResult UInventoryComponent::TryAddItem_Internal(UItemBase* ItemIn)
{
	// Call the function to add the item on the server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const int32 AddAmount = ItemIn->GetQuantityAsInt();
		
		// Check if the item can be added to the inventory
		if (Items.Num() + 1 > GetItemCapacity())
		{
			return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryCapacityFullText", "Couldn't add item to inventory.  Inventory is full."));
		}

		// Do a weight check on items that weigh more than zero
		if (!FMath::IsNearlyZero(ItemIn->ItemWeight))
		{
			// Check if adding the item will take the inventory over weight capacity, don't add it if it will
			if (GetCurrentWeight() + ItemIn->ItemWeight > GetWeightCapacity())
			{
				return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryTooMuchWeightText", "Couldn't add item to inventory.  Carrying too much weight."));
			}
		}
		
		if (ItemIn->bCanBeStacked)
		{
			// The quantity should never go over the max stack size.  Stop if this happens.
			ensure(ItemIn->GetQuantityAsInt() <= ItemIn->MaxStackSize);

			// If this item is stackable, check if the item already exists and try to add it to an existing stack
			if (TObjectPtr<UItemBase> ExistingItem = FindItem(ItemIn))
			{
				// Check if the ItemIn is less than the maximum amount ie has found 50 ammo but can only carry 20
				if (ExistingItem->GetQuantityAsInt() < ExistingItem->MaxStackSize)
				{
					// Work out how much the capacity will allow us to add
					const int32 CapacityMaxAddAmount = ExistingItem->MaxStackSize - ExistingItem->GetQuantityAsInt();
					// Work out the lesser of the amount to be added
					int32 ActualAddAmount = FMath::Min(AddAmount, CapacityMaxAddAmount);

					// Create some error text for when things go wrong
					FText ErrorText = LOCTEXT("InventoryErrorText", "Couldn't add item to the inventory");

					// Now that we know the quantity to be added, make sure we don't go over the weight capacity
					if (!FMath::IsNearlyZero(ItemIn->ItemWeight))
					{
						const int32 WeightMaxAddAmount = FMath::FloorToInt((WeightCapacity - GetCurrentWeight()) / ItemIn->ItemWeight);
						// Get the lesser of the amount to be added
						ActualAddAmount = FMath::Min(ActualAddAmount, WeightMaxAddAmount);
						// If we can't take the full amount due to weight, update the error text
						if (ActualAddAmount < AddAmount)
						{
							ErrorText = FText::Format(LOCTEXT("InventoryTooMuchWeight", "Couldn't take the full quantity of {ItemName} to the inventory.  Too much weight."), ItemIn->ItemName);
						}						
					}
					else if (ActualAddAmount < AddAmount)
					{
						// If the item weighs nothing but we can't take it due to capacity, update the error text
						ErrorText = FText::Format(LOCTEXT("InventoryCapacityFullText", "Couldn't take the full quantity of {ItemName} to the inventory.  Inventory is full."), ItemIn->ItemName);
					}

					// Check if anything can be added to this invetory
					if (ActualAddAmount <= 0)
					{
						return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryErrorText", "Couldn't add item to inventory."));
					}

					// Update the quantity in the inventory
					ExistingItem->SetQuantity(ExistingItem->GetQuantityAsInt() + ActualAddAmount);

					// Check we haven't gone over the max stack size, stop if it has
					ensure(ExistingItem->GetQuantityAsInt() <= ExistingItem->MaxStackSize);

					// Check if we were able to add the full amount to the stack
					if (ActualAddAmount < AddAmount)
					{
						return FItemAddResult::AddedSome(AddAmount, ActualAddAmount, ErrorText);
					}
					else
					{
						return FItemAddResult::AddedAll(AddAmount);
					}
				}
				else
				{
					return FItemAddResult::AddedNone(AddAmount, FText::Format(LOCTEXT("InventoryFullStackText", "Couldn't add {ItemName}.  You already have a full stack of this item."), ItemIn->ItemName));
				}
			}
			else // The item is stackable but we don't have any
			{
				AddItem(ItemIn);
				return FItemAddResult::AddedAll(AddAmount);
			}
		}
		else // The item is not stackable
			{
			ensure(ItemIn->GetQuantityAsInt() == 1);

			AddItem(ItemIn);
			return FItemAddResult::AddedAll(AddAmount);
			}
	}
	// This function should never be called on the server.  Check for this.
	check(false);
	return FItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

UItemBase* UInventoryComponent::AddItem(UItemBase* ItemToAdd)
{
	// Check whether the component has an owner and if that owner is the server since items can only be added by the server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// Recreate the item so that it becomes owned by the owner of this component
		TObjectPtr<UItemBase> NewItem = NewObject<UItemBase>(GetOwner(), ItemToAdd->GetClass());
		NewItem->SetQuantity(ItemToAdd->GetQuantityAsInt());
		NewItem->OwningInventory = this;
		NewItem->AddedToInventory(this);
		Items.Add(NewItem);
		NewItem->MarkDirtyForReplication();

		return NewItem;
	}

	// Return nothing if there is no owner or is not the server
	return nullptr;
}

#undef LOCTEXT_NAMESPACE