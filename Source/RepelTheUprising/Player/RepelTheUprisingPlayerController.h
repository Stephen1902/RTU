// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RepelTheUprisingPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryStatusChanged, bool, bNewStatus);

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API ARepelTheUprisingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInventoryStatusChanged OnInventoryStatusChanged;
protected:
	/** Called on game start */
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Inventory")
	TSubclassOf<UUserWidget> PlayerInventoryWidget;
	
	/** Called when toggling Inventory */
	void ToggleInventory();

private:
	/** Inventory Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InventoryAction;

	void SetupPlayerInventory();

	UPROPERTY()
	TObjectPtr<class UPlayerInventoryWidget> InventoryWidgetRef;
};
