// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RepelTheUprisingCharacter.generated.h"

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
public:
	ARepelTheUprisingCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/** Called on game start */
	virtual void BeginPlay() override;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called when the player moves the mouse to look */
	void StartTurn(float Value);

	/** Called the player jumps */
	void StartJump();

	/** Called when the player stops jumping */
	void EndJump();

	/** Called when the player holds the run key */
	void StartRunning();

	/** Called when the player releases the run key */
	void EndRunning();

	/** Called when the player climbs up a ledge */
	void ClimbUp();

	/** Called when the player drops down from a ledge */
	void DropDown();
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

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
};

