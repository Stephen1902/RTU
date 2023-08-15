// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Helper function to get the item in this inventory item widget
	UFUNCTION(BlueprintCallable, Category = "Inventory Item Widget")
	class UItemBase* GetItem() const { return Item; }
	
private:
	// The item for this widget
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item Widget", meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	TObjectPtr<UItemBase> Item;
};
