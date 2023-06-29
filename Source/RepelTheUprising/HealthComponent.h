// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// On health changed event
UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams( FOnHealthChanged, class UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser );

UCLASS( ClassGroup=(Components), meta=(BlueprintSpawnableComponent) )
class REPELTHEUPRISING_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Default / maximum health this item has
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Component")
	double DefaultHealth;

	// How much health this item regenerates
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Component")
	double HealthRegenPerSecond;

	// Delay between damage taking place and when health regen starts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Component")
	double TimeBeforeHealing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Component")
	double CurrentHealth;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
private:
	UFUNCTION()
	void HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	double TimeSinceLastDamage;

	void TryToHeal(float DeltaTime);
	
};
