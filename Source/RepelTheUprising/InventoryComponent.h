// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// Called when the inventory component is updated
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemsAdded	UMETA(DisplayName = "No Items Added"),
	IAR_SomeItemsAdded	UMETA(DisplayName = "Some Items Added"),
	IAR_AllItemsAdded	UMETA(DisplayName = "All Items Added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

public:
	FItemAddResult() {};
	FItemAddResult(int32 InItemQuantity) : AmountToGive(InItemQuantity), ActualAmountGiven(0) {};
	FItemAddResult(int32 InItemQuantity, int32 InQuantityAdded) : AmountToGive(InItemQuantity), ActualAmountGiven(InQuantityAdded) {};
 	
	// The amount of the item we tried to add
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 AmountToGive;

	// The amount of the item actually given, after stack size is factored in
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountGiven;

	// The result, taken from an Enum
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult Result;

	// If something went wrong, text to display explaining why
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ErrorText;

	// Helper Functions
	static FItemAddResult AddedNone(const int32 InItemQuantity, const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult(InItemQuantity);
		AddedNoneResult.Result = EItemAddResult::IAR_NoItemsAdded;
		AddedNoneResult.ErrorText = ErrorText;

		return AddedNoneResult;
	}

	static FItemAddResult AddedSome(const int32 InItemQuantity, const int32 ActualAmountGiven, const FText& ErrorText)
	{
		FItemAddResult AddedSomeResult(InItemQuantity, ActualAmountGiven);
		AddedSomeResult.Result = EItemAddResult::IAR_SomeItemsAdded;
		AddedSomeResult.ErrorText = ErrorText;

		return AddedSomeResult;
	}

	static FItemAddResult AddedAll(const int32 InItemQuantity)
	{
		FItemAddResult AddedAllResult(InItemQuantity, InItemQuantity);
		AddedAllResult.Result = EItemAddResult::IAR_AllItemsAdded;

		return AddedAllResult;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UItemBase;
public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	UFUNCTION(BlueprintPure, Category = "Inventory")
	double GetWeightCapacity() const { return WeightCapacity; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCapacity() const { return ItemCapacity; }
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItemBase*> GetInventoryItems() const { return Items; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();

	// Try to add an item to the inventory, if it already exists, only calls an internal function but used to not allow external access to that function
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemAddResult TryToAddItem(UItemBase* ItemToAdd);

	// Add an item to the inventory if it doesn't already exist
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemAddResult TryToAddItemFromClass(TSubclassOf<UItemBase> ItemClass, const int32 Quantity);

	// Remove an item from the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemBase* ItemToRemove);

	// CHeck if we have a given amount of an item
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(TSubclassOf<UItemBase> ItemClass, const int32 Quantity = 1);

	// Return the first item with the same class of a given item
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemBase* FindItem(UItemBase* ItemIn) const;

	// Return the first item with the same class as the class given
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemBase* FindItemByClass(TSubclassOf<UItemBase> ItemClass) const;

	// Get all items of a specific child of a class ie all weapons, all food etc.
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItemBase*> FindItemsByClass(TSubclassOf<UItemBase> ItemClass) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	double GetCurrentWeight() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetWeightCapacity(const double NewWeightCapacity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetCapacity(const int32 NewCapacity);

	// Take something away from this item and remove it, useful for things like eating, using ammo etc.
	int32 ConsumeItem(UItemBase* ItemIn);
	int32 ConsumeItem(UItemBase* ItemIn, const int32 QuantityIn);
protected:
	// The maximum weight this inventory can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	double WeightCapacity;

	// The maximum number of items this inventory can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta=(ClampMin= 1 , ClampMax = 200))
	int32 ItemCapacity;
	
	// The items being held in this component
	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, Category = "Inventory")
	TArray<TObjectPtr<UItemBase>> Items;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
private:
	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;

	// Internal function for adding items to the inventory
	FItemAddResult TryAddItem_Internal(UItemBase* ItemIn);

	// Function to actually add the item to the inventory
	UItemBase* AddItem(class UItemBase* ItemToAdd);
};
