// Copyright 2023 DME Games

#include "RepelTheUprisingPlayerController.h"
#include "../InventoryComponent.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "RepelTheUprising/PlayerInventoryWidget.h"

void ARepelTheUprisingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupPlayerInventory();
}

void ARepelTheUprisingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up gameplay key bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Inventory
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ARepelTheUprisingPlayerController::ToggleInventory);
	} 
}

void ARepelTheUprisingPlayerController::ToggleInventory()
{
	if (IsLocalController() && InventoryWidgetRef)
	{
		if (!InventoryWidgetRef->IsInViewport())
		{
			InventoryWidgetRef->AddToViewport();
			SetInputMode(FInputModeGameAndUI());
			OnInventoryStatusChanged.Broadcast(true);
			SetShowMouseCursor(true);
		}
		else
		{
			InventoryWidgetRef->RemoveFromParent();
			SetInputMode(FInputModeGameOnly());
			OnInventoryStatusChanged.Broadcast(false);
			SetShowMouseCursor(false);
		}
	}
}

void ARepelTheUprisingPlayerController::SetupPlayerInventory()
{
	// The widget should only be displayed on the clients, not the server
	if (IsLocalController() && PlayerInventoryWidget)
	{
		if (APlayerController* PC = Cast<APlayerController>(this))
		{
			InventoryWidgetRef = CreateWidget<UPlayerInventoryWidget>(PC, PlayerInventoryWidget);
		}
	}
}
