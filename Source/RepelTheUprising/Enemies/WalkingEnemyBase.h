// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WalkingEnemyBase.generated.h"


UENUM(BlueprintType)
enum class EWalkingEnemyStatus : uint8
{
	EES_Disabled		UMETA(DisplayName="Disabled"),
	EES_Normal			UMETA(DisplayName="Normal"),
	EES_Alert			UMETA(DisplayName="Alert"),
	EES_Chasing			UMETA(DisplayName="Chasing"),
	EES_Combat			UMETA(DisplayName="Combat")
};


UCLASS()
class REPELTHEUPRISING_API AWalkingEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWalkingEnemyBase();

	virtual void SetNewStatus(EWalkingEnemyStatus NewStatusIn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Current status of this enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	EWalkingEnemyStatus EnemyStatus;

	// All enemies will have health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	class UHealthComponent* HealthComp;

	// Skin colour of this enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor SkinColour;

	// Colour of this enemy status light when normal
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor NormalStatusColour;

	// Colour of this enemy status light when on alert
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor AlertStatusColour;

	// Colour of this enemy status light when chasing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor ChasingStatusColour;

	// Colour of this enemy status light when in combat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor CombatStatusColour;

	// Colour of this enemy status light when disabled
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor DisabledStatusColour;

	// Locations this enemy will patrol between when it has no player to chase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up")
	TArray<AActor*> PatrolPoints;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called on Begin Play to get the first location for the enemy to move to
	FVector GetFirstPatrolPoint();
	
	// Called to get the next Patrol Point, if one exists
	FVector GetNextPatrolPoint();

	// Gets length of the PatrolPoints array
	int32 GetLengthOfPPArray() const { return PatrolPoints.Num(); }

private:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnStatusChanged();
	
	UPROPERTY(Replicated)
	float CurrentMovementSpeed;

	int32 CurrentPatrolPoint;
};
