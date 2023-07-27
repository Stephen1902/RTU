// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void InitializeInventory(class UInventoryComponent* InventoryIn);
};
