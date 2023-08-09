// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpItemBase.generated.h"

USTRUCT()
struct FThisItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	UStaticMesh* MeshToDisplay;

	// Maximum health this item can be repaired to
	UPROPERTY()
	double MaxHealth;

	// 	
	UPROPERTY()
	double ItemCurrentHealth;
};

UCLASS()
class REPELTHEUPRISING_API APickUpItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItemBase();

	void SetItemInfo(UStaticMesh* MeshIn, double MaxHealthIn, double CurrentHealthIn)
	
protected:
	// Static mesh to display for this item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Set Up")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	// Interaction component to display widget on screen to pick up etc.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Set Up")
	TObjectPtr<class UInteractionComponent> InteractionComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
};
