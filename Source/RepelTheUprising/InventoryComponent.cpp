// Copyright 2023 DME Games


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Items/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


FItemAddResult UInventoryComponent::TryToAddItem(UItemBase* ItemToAdd)
{
	return TryAddItem_Internal(ItemToAdd);
}

FItemAddResult UInventoryComponent::TryToAddNewItem(TSubclassOf<UItemBase> ItemClass, const int32 Quantity)
{
	UItemBase* NewItem = NewObject<UItemBase>(GetOwner(), ItemClass);
	NewItem->SetQuantity(Quantity);
	return TryAddItem_Internal(NewItem);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
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
	// Replicate the items to the clients, if required
	
}

FItemAddResult UInventoryComponent::TryAddItem_Internal(UItemBase* Item)
{
	Items.Add(Item);
	return FItemAddResult::AddedAll(Item->CurrentQuantity);
}

UItemBase* UInventoryComponent::AddItem(UItemBase* ItemToAdd)
{
	// TODO amend as needed
	return ItemToAdd;
}
