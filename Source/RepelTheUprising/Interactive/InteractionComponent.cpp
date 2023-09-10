// Copyright 2023 DME Games

#include "InteractionComponent.h"
#include "InteractionWidget.h"
#include "../Player/RepelTheUprisingCharacter.h"

UInteractionComponent::UInteractionComponent()
{
	// This component does not need to tick, set to false to improve performance
	UActorComponent::SetComponentTickEnabled(false);

	// Set up default parameters
	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	InteractionNameText = FText::FromString("Interaction Object");
	InteractionUseText = FText::FromString("Interact");
	bAcceptMultipleUsers = false;

	// Set the widget so it only appears in local space at a specific size
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(600, 100);
	bDrawAtDesiredSize = true;

	// Set the component as active but keep it hidden
	UActorComponent::SetActive(true);
	SetHiddenInGame(true);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractionNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractionNameText = NewActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for (int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (TObjectPtr<ARepelTheUprisingCharacter> Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ARepelTheUprisingCharacter* Character) const
{
	const bool bPlayerAlreadyInteracting = !bAcceptMultipleUsers && Interactors.Num() >= 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::RefreshWidget()
{
	//Make sure the widget is initialized, and that we are displaying the right values (these may have changed)
	if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
	{
		InteractionWidget->UpdateInteractionWidget(this);
	}
}

void UInteractionComponent::BeginFocus(ARepelTheUprisingCharacter* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	OnBeginFocus.Broadcast(Character);

	if (GetNetMode() != NM_DedicatedServer)
	{
		SetHiddenInGame(false);

		// Get all primitive components attached to the viewed actor
		TArray<UPrimitiveComponent*> PrimitiveComponents;
		GetOwner()->GetComponents(PrimitiveComponents, false);
		for (const auto& VisualComp : PrimitiveComponents)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(true);
			}
		}
	}

	RefreshWidget();
}

void UInteractionComponent::EndFocus(ARepelTheUprisingCharacter* Character)
{
	OnEndFocus.Broadcast(Character);

	if (GetNetMode() != NM_DedicatedServer)
	{
		SetHiddenInGame(true);

		TArray<TObjectPtr<UPrimitiveComponent>> PrimitiveComponents;
		GetOwner()->GetComponents(PrimitiveComponents, false);
		for (auto& VisualComp : PrimitiveComponents)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
}

void UInteractionComponent::BeginInteract(ARepelTheUprisingCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ARepelTheUprisingCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ARepelTheUprisingCharacter* Character)
{
	if (CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

float UInteractionComponent::GetInteractPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (TObjectPtr<ARepelTheUprisingCharacter> Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}
	return 0.f;
}