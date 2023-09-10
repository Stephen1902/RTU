// Copyright 2023 DME Games


#include "PickUpItemBase.h"
#include "ItemBase.h"
#include "RepelTheUprising/Player/RepelTheUprisingCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "RepelTheUprising/InventoryComponent.h"
#include "RepelTheUprising/Interactive/InteractionComponent.h"

// Sets default values
APickUpItemBase::APickUpItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Display Mesh"));
	SetRootComponent(DisplayMesh);
	DisplayMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));
	InteractionComponent->InteractionTime = 0.5;
	InteractionComponent->InteractionDistance = 200.;
	InteractionComponent->InteractionNameText = FText::FromString("Pickup");
	InteractionComponent->InteractionUseText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &APickUpItemBase::OnTakePickup);
	InteractionComponent->SetupAttachment(DisplayMesh);

	// This actor needs to replicate or clients will not be able to use it
	SetReplicates(true);
}

void APickUpItemBase::InitializePickup(const TSubclassOf<UItemBase> ItemClass, const int32 Quantity)
{
	if (HasAuthority() && ItemClass && Quantity > 0)
	{
		Item = NewObject<UItemBase>(this, ItemClass);
		Item->SetQuantity(Quantity);

		OnRep_Item();
		Item->MarkDirtyForReplication();
	}
}

void APickUpItemBase::OnRep_Item()
{
	if (Item)
	{
		DisplayMesh->SetStaticMesh(Item->DisplayMesh);
		InteractionComponent->InteractionNameText = Item->ItemName;

		// Client needs to bind the OnItemModified delegate in order to refresh the widget if the quantity changes
		Item->OnItemModified.AddDynamic(this, &APickUpItemBase::OnItemModified);
	}

	// Refresh the widget for this item since something has changed
	InteractionComponent->RefreshWidget();
}

void APickUpItemBase::OnItemModified()
{
	if (InteractionComponent)
	{
		InteractionComponent->RefreshWidget();
	}
}

// Called when the game starts or when spawned
void APickUpItemBase::BeginPlay()
{
	Super::BeginPlay();

	// bNetStartup is true if item was placed in the level, false if spawned by something dropping it 
	if (HasAuthority() && ItemTemplate && bNetStartup)
	{
		InitializePickup(ItemTemplate->GetClass(), ItemTemplate->GetQuantityAsInt());
	}

	// If the item was spawned in at runtime, call to make sure it is rotated correctly in the world
	if (!bNetStartup)
	{
		AlignWithGround();
	}

	if (Item)
	{
		Item->MarkDirtyForReplication();
	}
}

void APickUpItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickUpItemBase, Item);
}

bool APickUpItemBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->GetRepKey()))
	{
		bWroteSomething |= Channel->ReplicateSubobject(Item,*Bunch, *RepFlags);
	}

	return bWroteSomething;
}

#if WITH_EDITOR
void APickUpItemBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// If a new pickup has been selected in the Editor, change the mesh to the new item
	if (PropertyName == GET_MEMBER_NAME_CHECKED(APickUpItemBase, ItemTemplate))
	{
		if (ItemTemplate)
		{
			DisplayMesh->SetStaticMesh(ItemTemplate->DisplayMesh);
		}
	}
}
#endif

void APickUpItemBase::OnTakePickup(ARepelTheUprisingCharacter* Taker)
{
	if (!Taker)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s was taken but the player was not valid"), *GetName());
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s %s, %s"), *Taker->GetName(), HasAuthority() ? TEXT("Has authority") : TEXT("Does not have authority"), Item != nullptr ? TEXT("Has item") : TEXT("Does not have item"));
	}
	
	// IsPendingKillPending should ensure the interact key cannot be spammed to pickup multiple instances
	if (HasAuthority() && !IsPendingKillPending() && Item)
	{
		// Check that the Taker has a valid inventory component and add this pickup to it
		if (const TObjectPtr<UInventoryComponent> PlayerInventory = Taker->GetPlayerInventory())
		{
			const FItemAddResult AddResult = PlayerInventory->TryToAddItem(Item);

			// Check if there is any quantity left in this pick up, amend if yes, destroy if no
			if (AddResult.ActualAmountGiven < Item->GetQuantityAsInt())
			{
				Item->SetQuantity(Item->GetQuantityAsInt() - AddResult.ActualAmountGiven);
			}
			else
			{
				Destroy();
			}
		}
	}
}