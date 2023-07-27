// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	UFUNCTION(BlueprintPure, Category = "Inventory")
	double GetWeightCapacity() const { return WeightCapacity; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCapacity() const { return ItemCapacity; }
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<class UItemBase*> GetInventoryItems() const { return Items; }
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// The maximum weight this inventory can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	double WeightCapacity;

	// The maximum number of items this inventory can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta=(ClampMin= 1 , ClampMax = 200))
	int32 ItemCapacity;
	
	// The items being held in this component
	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, Category = "Inventoty")
	TArray<TObjectPtr<UItemBase>> Items;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
private:
	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;
	
};
