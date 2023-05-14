// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class ARepelTheUprisingCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class ARepelTheUprisingCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class ARepelTheUprisingCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class ARepelTheUprisingCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class ARepelTheUprisingCharacter*, Character);

UCLASS(ClassGroup = (CUSTOM), meta = (BlueprintSpawnableComponent))
class REPELTHEUPRISING_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	// The time required for the player to interact with this component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	double InteractionTime;

	// Distance the player can interact with this component from
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	double InteractionDistance;

	// Name of this item to be displayed on screen
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionNameText;

	// Text to be displayed which explains the action ie "Turn On" with a lamp
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionUseText;

	// Whether or not more than one player can interact at any one time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAcceptMultipleUsers;

	//Call this to change the name of the interactable. Will also refresh the interaction widget.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableActionText(const FText& NewActionText);

	//Delegates

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	//[local + server] Called when the player has interacted with the item for the required amount of time
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnInteract;

protected:
	// Called when the game starts
	virtual void Deactivate() override;

	bool CanInteract(class ARepelTheUprisingCharacter* Character) const;

	//On the server, this will hold all interactors. On the local player, this will just hold the local player (provided they are an interactor)
	UPROPERTY()
	TArray<class ARepelTheUprisingCharacter*> Interactors;

public:

	/***Refresh the interaction widget and its custom widgets.
	An example of when we'd use this is when we take 3 items out of a stack of 10, and we need to update the widget
	so it shows the stack as having 7 items left. */
	void RefreshWidget();

	//Called on the client when the players interaction check trace begins/ends hitting this item
	void BeginFocus(class ARepelTheUprisingCharacter* Character);
	void EndFocus(class ARepelTheUprisingCharacter* Character);

	//Called on the client when the player begins/ends interaction with the item
	void BeginInteract(class ARepelTheUprisingCharacter* Character);
	void EndInteract(class ARepelTheUprisingCharacter* Character);

	void Interact(class ARepelTheUprisingCharacter* Character);

	//Return a value from 0-1 denoting how far through the interact we are. 
	//On server this is the first interactors percentage, on client this is the local interactors percentage
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractPercentage();
};
