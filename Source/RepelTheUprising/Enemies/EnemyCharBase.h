// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyCharBase.generated.h"

/**
 * Base class for all enemies that can walk around
 */
UCLASS()
class REPELTHEUPRISING_API AEnemyCharBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyCharBase();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// The skeletal mesh of this character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up")
	TObjectPtr<USkeletalMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Set Up")
	TObjectPtr<class UCharacterMovementComponent> MovementComp;
	
	// Speed at which this enemy moves when walking / patrolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float DefaultWalkSpeed;

	// Speed at which the enemy moves when running / chasing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float DefaultRunSpeed;

	
	
private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	float CurrentMovementSpeed;
};

