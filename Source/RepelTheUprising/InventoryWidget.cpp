// Copyright 2023 DME Games


#include "InventoryWidget.h"

#include "InventoryComponent.h"

void UInventoryWidget::InitializeInventory(UInventoryComponent* InventoryIn)
{
	UInventoryComponent* CurrentInventory = InventoryIn;
	TArray<UItemBase*> ItemsInInventory = InventoryIn->GetInventoryItems();
}
