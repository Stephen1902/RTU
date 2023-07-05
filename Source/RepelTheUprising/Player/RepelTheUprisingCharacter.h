// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RepelTheUprisingCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaUpdated, class ARepelTheUprisingCharacter*, PlayerCharacter, float, NewStamina);

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY();

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.0;
		bIsInteractHeld = false;
	}

	// The currently viewed component
	UPROPERTY()
	TObjectPtr<class UInteractionComponent> ViewedInteractionComponent;

	// Time since we last checked for an interaction component
	UPROPERTY()
	double LastInteractionCheckTime;

	// Whether the local player is holding the interact key
	UPROPERTY()
	bool bIsInteractHeld;
};

UCLASS(config=Game)
class ARepelTheUprisingCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* BodyMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* FeetMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* BackpackMesh;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	class UStaminaComponent* StaminaComponent;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	class UFoodComponent* FoodComponent;

	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	class UWaterComponent* WaterComponent;
	
	// Montage to be used when the player is hanging from a ledge
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	UAnimMontage* HangMontage;

	// Difference between where the player is and where they can get to when against a climbable ledge
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	float MaximumWallClimbDistance = -50.f;

	// Amount of time to check if a player can hang and climb by default
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	float DefaultClimbCheckTime = 0.5f;

	// Amount of time to check if a player can hang and climb when running
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	float RunningClimbCheckTime = 1.0f;

	// Whether the character should auto climb walls or wait for player input 
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	bool bShouldAutoClimb = true;

	// Amount of time before the auto climb happens
	UPROPERTY(EditDefaultsOnly, Category="Climbing", meta=(AllowPrivateAccess="true"))
	float TimeBeforeAutoClimb = 0.2f;

	/** In game widgets the player will see on their screen */
	// In game widget
	UPROPERTY(EditDefaultsOnly, Category="Set Up", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> MainWidget;  

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	/** Run Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;

	/** Climb Up Action Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ClimbUpAction;

	/** Drop Down Action Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DropDownAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	// Speed multiplier when player is running
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running, meta = (AllowPrivateAccess = "true"))
	double MaxSpeedMultiplier = 1.5;

	// Times between interaction checks, set to 0 if checking every frame
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	double TimeBetweenInteractionChecks = 0.0;

	// Distance in front of the player to check for interactive items
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	double LineTraceDistance = 300.0;

	UFUNCTION()
	void OnHealthIsChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	ARepelTheUprisingCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Called when the player jumps */
	void StartJump();

	/** Called when the player stops jumping */
	void EndJump();

	void SetClimbCheckTime(const float CheckTimeIn) { ClimbCheckTime = CheckTimeIn; }

	//True if we're interacting with an item that has an interaction time (for example a lamp that takes 2 seconds to turn on)
	bool IsInteracting() const;

	//Get the time till we interact with the current interaction component
	float GetRemainingInteractTime() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	UPROPERTY(BlueprintAssignable)
	FOnStaminaUpdated OnStaminaUpdated;

protected:
	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/** Called on game start */
	virtual void BeginPlay() override;

	/** Called when the player climbs up a ledge */
	void ClimbUp();

	/** Called when the player drops down from a ledge */
	void DropDown();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called when toggling */
	void ToggleCrouching();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UFUNCTION(Server, Reliable)
	void Server_ModifySprintStatus(bool bNewSprintStatus);

private:
	/** Called for sprint input */
	void BeginSprint(const FInputActionValue& Value);
	void EndSprint(const FInputActionValue& Value);
	void ModifySprintStatus(bool bNewSprintStatus);
	UFUNCTION()
	void OnStaminaChange(float CurrentStaminaIn);

	bool bIsCrouching = false;

	// Variables for jumping and grabbing a ledge
	float ClimbCheckTime = 0.f;
	bool bIsClimbing = false;
	FTimerHandle WallClimbTimer;
	// Check to ensure climb key cannot be spammed
	bool bFinishedClimbing = false;
	FVector HeightLocation;
	FVector ForwardHitNormal;
	FVector ForwardHitLocation;

	// Do sphere trace forward to check for climbing options
	void DoForwardSphereTrace();

	// Do sphere trace up to check for climbing options
	void DoVerticalSphereTrace();

	// Called when a player can climb and presses the jump button
	void Hang();

	UFUNCTION()
	void OnAnimNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	float DefaultWalkSpeed;
	float MaxRunSpeed;
	UPROPERTY(Replicated)
	bool bIsRunning = false;

	// Things to be put in Begin Play
	void SetEnhancedSubsystem() const;
	void SetDefaultVariables();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	// For Interactions
	UPROPERTY()
	FInteractionData InteractionData;
	// Helper function to easily get the viewed interaction component
	UInteractionComponent* GetInteractionComp() const { return InteractionData.ViewedInteractionComponent; }
	TObjectPtr<class UInteractionComponent> InteractiveRef = nullptr;
	FTimerHandle TimerHandle_Interact;
	double TimeSinceLastInteraction = 0.0;
	void DoInteractionCheck();
	void CouldntFindInteractionComp();
	void FoundNewInteractionComp(TObjectPtr<UInteractionComponent> InteractionCompIn);
	void BeginInteract();
	void EndInteract();

	// Current Level of this character, used with a data table to update other information
	int32 CurrentLevel;
	
	UFUNCTION(Server, Reliable)
	void Server_SetMovementVariables();
	
	// For on screen widgets
	TObjectPtr<class UPlayerInGameWidget> MainWidgetRef;
	void CreatePlayerWidgets();
};

