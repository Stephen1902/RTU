// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, class UStaminaComponent*, StaminaComponent, double, NewStamina);

UCLASS( ClassGroup=(Components), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ModifyStamina(float Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina)
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaRegenerationRate;

	// Time after stamina is used before regeneration occurs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float TimeBeforeRegeneration;

	UFUNCTION()
	void OnRep_CurrentStamina();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;

	float TimeSinceStaminaUsed;
	
};
