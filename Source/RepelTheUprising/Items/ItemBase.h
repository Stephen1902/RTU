// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemModified);

/**
 * 
 */

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Misc		UMETA(DisplayName="Misc"),
	EIT_Food		UMETA(DisplayName="Food"),
	EIT_Weapon		UMETA(DisplayName="Weapon"),
	EIT_Ammo		UMETA(DisplayName="Ammo"),
	EIT_Health		UMETA(DisplayName="Health"),
	EIT_Book		UMETA(DisplayName="Book"),
	EIT_Tool		UMETA(DisplayName="Tool"),
	EIT_Building	UMETA(DisplayName="Building"),
	EIT_Addition	UMETA(DisplayName="ObjectAddition")  // Items that are added to another item ie grill in a campfire
};

USTRUCT(BlueprintType)
struct FItemSpoils
{
	GENERATED_BODY()

	// Whether or not this item will eventually destroy itself
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	bool bCanSpoil;

	// Time in game hours before the spoiling happens
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bCanSpoil"))
	double SpoilTime;

	// The item this becomes when it has spoiled
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bCanSpoil"))
	FName ItemWhenSpoiled;

	FItemSpoils()
	{
		bCanSpoil = false;
		SpoilTime = 0.f;
		ItemWhenSpoiled = FName("MISC SPOILED");
	}
};

USTRUCT(BlueprintType)
struct FItemCraftingInfo
{
	GENERATED_BODY()

	// Whether or not this item is crafted and shows in the crafting UI
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	bool bIsCrafted;

	// Items and quantities of things that make up this crafted item
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	TMap<FName, int32>  CraftedItemIngredients;

	// Minimum that gets made when crafted
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	int32 CraftedMadeMinimum;

	// Maximum that gets made when crafted
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	int32 CraftedMadeMaximum;

	// Item needed to display this object (Campfire, Workbench, Forge etc.) in the UI
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	FName ItemNeededToCraftMain;
	
	// Item needed to craft this (Saucepan, Grill, Anvil etc.) inside the ItemNeededToCraftMain
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	FName ItemNeededToCraftSecondary;

	// Amount of time needed to craft this item
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true", EditCondition="bIsCrafted"))
	double CraftingTime;
	
	FItemCraftingInfo()
	{
		bIsCrafted = false;
		CraftedMadeMinimum = 1;
		CraftedMadeMaximum = 1;
		CraftingTime = 1.0;
	}
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class REPELTHEUPRISING_API UItemBase : public UObject
{
	GENERATED_BODY()
	
protected:
	// Functions for networking and replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	
	UItemBase();

	// Item value of this item, must be unique for the DataTable
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FName UniqueID;
	
	// Name of this item, as displayed in the world and in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FText ItemName;

	UPROPERTY(EditAnywhere, Category = "Set Up")
	EItemType ItemType;

	// Description of this item, as displayed in the in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FText ItemDescription;

	// Text to be displayed by the in game UI for how this item is used
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FText ItemUseText;
	
	// Display texture for in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up")
	TObjectPtr<UTexture2D> ItemTexture;

	// Base cost of this item when buying or selling
	UPROPERTY(EditAnywhere, Category = "Set Up")
	double ItemBaseCost;
/*
	// Display mesh for this item when placed in the world
	UPROPERTY(EditAnywhere, Category = "Set Up")
	TObjectPtr<UStaticMesh> DisplayMesh;
	
	// Crafted Items Information
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FItemCraftingInfo ItemCraftingInfo;

	// Info for items that spoil
	UPROPERTY(EditAnywhere, Category = "Set Up")
	FItemSpoils ItemSpoilInfo;
*/
	// Weight for one of these items
	UPROPERTY(EditAnywhere, Category = "Set Up")
	double ItemWeight;

	// Whether or not multiple units of this item can be stacked
	UPROPERTY(EditAnywhere, Category = "Set Up")
	bool bCanBeStacked;
	
	// Maximum amount this object will stack in an inventory
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(EditCondition = "bCanBeStacked", ClampMin = 2))
	int32 MaxStackSize;

	// Health this item has when created.  
	UPROPERTY(EditAnywhere, Category = "Set Up")
	double MaxHealth;
	
	// The quality level of this item
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(ClampMin = 0, ClampMax = 600));
	int32 ItemLevel;

	// The tooltip class to be displayed by this item
	UPROPERTY(EditAnywhere, Category = "Set Up")
	TSubclassOf<class UItemTooltip> ItemTooltip;

	// The inventory that owns this item
	UPROPERTY()
	TObjectPtr<class UInventoryComponent> OwningInventory;

	/*
	double TimeUntilSpoiled;
	FTimerHandle SpoilTimerHandle;

	double TimeUntilCrafted;
	FTimerHandle CraftTimerHandle;
*/
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	double CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentQty, EditAnywhere, Category = "Game Items", meta=(UIMin = 1, EditCondition = "bCanBeStacked"))
	int32 CurrentQuantity;

	UFUNCTION()
	void OnRep_CurrentQty();

	// Used to replicated items efficiently
	UPROPERTY()
	int32 RepKey;

	// Broadcast when an item is changed, used for updating Use Text etc.
	UPROPERTY(BlueprintAssignable)
	FOnItemModified OnItemModified;

	virtual void Use(TObjectPtr<class ARepelTheUprisingCharacter> Character);
	virtual void AddedToInventory(TObjectPtr<UInventoryComponent> Inventory);

	// Function to set a new health for an item
	UFUNCTION(BlueprintCallable, Category = "Game Items")
	void SetHealth(const int32 NewHealth);
	
	// Function to set a new quantity for an item
	UFUNCTION(BlueprintCallable, Category = "Game Items")
	void SetQuantity(const int32 NewQuantity);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Game Items")
	float GetStackWeight() const { return ItemWeight * CurrentQuantity; }

	UFUNCTION(BlueprintPure, Category = "Game Items")
	virtual bool ShouldShowInInventory() const;

private:
	// Called whenever something is changed to tell the server to update replicated quantities
	void MarkDirtyForReplication();
};
