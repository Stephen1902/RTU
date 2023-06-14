// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Misc	UMETA(DisplayName="Misc"),
	EIT_Food	UMETA(DisplayName="Food"),
	EIT_Weapon	UMETA(DisplayName="Weapon"),
	EIT_Ammo	UMETA(DisplayName="Ammo"),
	EIT_Health	UMETA(DisplayName="Health")
	
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
		ItemWhenSpoiled = TEXT("MISCSPOILED");
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
	
	
};

UCLASS()
class REPELTHEUPRISING_API UItemBase : public UObject
{
	GENERATED_BODY()

private:
	// Item value of this item, must be unique for the DataTable
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	FName UniqueID;
	
	// Name of this item, as displayed in the world and in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	FName ItemName;

	// Description of this item, as displayed in the in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	FName ItemDescription;

	// Display texture for in game UI
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTexture2D> ItemTexture;

	// Base cost of this item when buying or selling
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	double ItemBaseCost;

	// Crafted Items Information
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	FItemCraftingInfo ItemCraftingInfo;

	// Info for items that spoil
	UPROPERTY(EditAnywhere, Category = "Set Up", meta=(AllowPrivateAccess="true"))
	FItemSpoils ItemSpoilInfo;

	double TimeUntilSpoiled;
	FTimerHandle SpoilTimerHandle;

	double TimeUntilCrafted;
	FTimerHandle CraftTimerHandle;
};
