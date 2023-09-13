// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyStatus : uint8
{
	EES_Disabled		UMETA(DisplayName="Disabled"),
	EES_Normal			UMETA(DisplayName="Normal"),
	EES_Alert			UMETA(DisplayName="Alert"),
	EES_Chasing			UMETA(DisplayName="Chasing"),
	EES_Combat			UMETA(DisplayName="Combat")
};

/* Base class for all enemies in the game with the most common variables for them */
UCLASS()
class REPELTHEUPRISING_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Up")
	TObjectPtr<class UCapsuleComponent> RootComp;
	
	// All enemies will have health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	class UHealthComponent* HealthComp;

	// Colour of this enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Set Up")
	FLinearColor SkinColour;

	// Current status of this enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	EEnemyStatus EnemyStatus;

	// Time in seconds before a disabled enemy will return to an active state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeReset;

	// Time in seconds before an alerted enemy returns to a normal state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeAlertOff;

	// Time in seconds before an enemy chasing will stop and return to an Alert state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	double TimeBeforeCallOffChase;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
};
