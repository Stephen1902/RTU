// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItemBase.generated.h"

USTRUCT()
struct FThisItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	UStaticMesh* MeshToDisplay;

	// Maximum health this item can be repaired to
	UPROPERTY()
	double MaxHealth;

	// 	Current Health for this item
	UPROPERTY()
	double ItemCurrentHealth;
};

UCLASS(ClassGroup = (Items), Blueprintable, Abstract)
class REPELTHEUPRISING_API APickUpItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItemBase();

	// This is used as a template to create the pickup when spawn in the world
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TObjectPtr<class UItemBase> ItemTemplate;

	// Called on BeginPlay, take the item and creates an in world pickup from it
	void InitializePickup(const TSubclassOf<UItemBase> ItemClass, const int32 Quantity);

	// Function to help align this pickup with the ground
	UFUNCTION(BlueprintImplementableEvent)
	void AlignWithGround();
protected:
	// The item that will be added to the inventory when this pickup is taken
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing = OnRep_Item)
	TObjectPtr<UItemBase> Item;

	UFUNCTION()
	void OnRep_Item();

	// If a property on the item is changed, call this function to refresh the UI
	UFUNCTION()
	void OnItemModified();
	
	// Static mesh to display for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Set Up")
	TObjectPtr<UStaticMeshComponent> DisplayMesh;

	// Interaction component to display widget on screen to pick up etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Set Up")
	TObjectPtr<class UInteractionComponent> InteractionComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Network functions
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Called when a player takes a pickup
	UFUNCTION()
	void OnTakePickup(class ARepelTheUprisingCharacter* Taker);

};
