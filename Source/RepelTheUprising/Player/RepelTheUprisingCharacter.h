// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RepelTheUprisingCharacter.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY();

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
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

	// Speed multiplier when player is running
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Running, meta = (AllowPrivateAccess = "true"))
	double MaxSpeedMultiplier = 1.5f;

	// Times between interaction checks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	double TimeBetweenInteractionChecks = 0.1f;

	// Distance in front of the player to check for interactive items
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	double LineTraceDistance = 300.f;
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
	
	/** Called when the player holds the run key */
	void StartRunning();

	/** Called when the player releases the run key */
	void EndRunning();

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

private:
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
	bool bIsRunning = false;

	// Things to be put in Begin Play
	void SetEnhancedSubsystem() const;
	void GetAnimInst();
	void SetDefaultVariables();

	// For Interactions
	UPROPERTY()
	FInteractionData InteractionData;
	// Helper function to easily get the viewed interaction component
	UInteractionComponent* GetInteractionComp() const { return InteractionData.ViewedInteractionComponent; }
	TObjectPtr<class UInteractionComponent> InteractiveRef = nullptr;
	FTimerHandle TimerHandle_Interact;
	double TimeSinceLastInteraction = 0.f;
	void DoInteractionCheck();
	void CouldntFindInteractionComp();
	void FoundNewInteractionComp(TObjectPtr<UInteractionComponent> InteractionCompIn);
	void BeginInteract();
	void EndInteract();
};

