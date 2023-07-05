// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

UENUM()
enum class EStaminaModifier : uint8
{
	SM_Poison		UMETA(DisplayName = "Poisoned"),
	SM_Environment	UMETA(DisplayName = "Enviromental"),
	SM_Caffeine		UMETA(DisplayName = "Caffeine"),
	
};

USTRUCT(BlueprintType)
struct FStaminaModifier
{
	GENERATED_BODY()

	// The type of item that has modified the stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina Struct")
	EStaminaModifier StaminaModifier;

	// Whether the modifier adds or removes to the multiplier
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina Struct")
	bool bAddToStaminaMultiplier;

	// Whether the modifier is permanent or timed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina Struct")
	bool bUsesTimer;

	// Timer Handle for this structure
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina Struct")
	FTimerHandle StaminaStructTimerHandle;

	
};

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	double MaxStamina;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentStamina)
	double CurrentStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	double DefaultStaminaDrainRate;

	// Regeneration rate for the actor using this Component when the game starts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	double DefaultStaminaRegenerationRate;

	// Time after the actor using this Component stops using stamina before regeneration begins
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	double TimeBeforeStaminaRegen;

	void SetStaminaShouldDrain(bool bNewStateIn);
	void AdjustStaminaModifier(double StaminaAdjustmentIn);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStaminaChanged OnStaminaChanged;

	double GetCurrentStamina() const { return CurrentStamina; }
protected:
	UFUNCTION()
	void OnRep_CurrentStamina();

	void DrainStamina(float DeltaTime);
	void RegenerateStamina(float DeltaTime);
	
	bool bStaminaIsDraining;

	double CurrentStaminaDrainRate;
	double CurrentStaminaRegenerationRate;
	// To be used when the actor using this Component is poisoned, thirsty or taken some food/drink that affects regeneration
	double StaminaMultiplier;

	double TimeSinceStaminaUsed;
};