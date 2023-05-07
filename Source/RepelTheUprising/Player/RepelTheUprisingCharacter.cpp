// Copyright Epic Games, Inc. All Rights Reserved.

#include "RepelTheUprisingCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#define COLLISION_CanClimb ECC_GameTraceChannel1

//////////////////////////////////////////////////////////////////////////
// ARepelTheUprisingCharacter

ARepelTheUprisingCharacter::ARepelTheUprisingCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// bCanFly Must be set to true or the player will not be able to climb walls 
	GetCharacterMovement()->NavAgentProps.bCanFly = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(GetMesh(), FName("CameraSocket"));
	CameraComp->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastHiddenShadow(true);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetupAttachment(GetMesh());
	BodyMesh->SetMasterPoseComponent(GetMesh());
	
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands Mesh"));
	HandsMesh->SetupAttachment(GetMesh());
	HandsMesh->SetMasterPoseComponent(GetMesh());
	
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs Mesh"));
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->SetMasterPoseComponent(GetMesh());
	
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet Mesh"));
	FeetMesh->SetupAttachment(GetMesh());
	FeetMesh->SetMasterPoseComponent(GetMesh());
	
	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet Mesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	HelmetMesh->SetMasterPoseComponent(GetMesh());
	
	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack Mesh"));
	BackpackMesh->SetupAttachment(GetMesh());
	BackpackMesh->SetMasterPoseComponent(GetMesh());
	
	//////////////////////////////////////////////////////////////////////////
	// Input
}

void ARepelTheUprisingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARepelTheUprisingCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARepelTheUprisingCharacter::EndJump);

	PlayerInputComponent->BindAction("ClimbUp", IE_Pressed, this, &ARepelTheUprisingCharacter::ClimbUp);
	PlayerInputComponent->BindAction("DropDown", IE_Pressed, this, &ARepelTheUprisingCharacter::DropDown);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ARepelTheUprisingCharacter::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ARepelTheUprisingCharacter::EndRunning);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARepelTheUprisingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARepelTheUprisingCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turn rate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ARepelTheUprisingCharacter::StartTurn);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARepelTheUprisingCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARepelTheUprisingCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARepelTheUprisingCharacter::ToggleCrouching);
	
}

void ARepelTheUprisingCharacter::DoForwardSphereTrace()
{
	FHitResult HitResult;
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = (GetActorForwardVector() * 150.f) + TraceStart;
	TArray<AActor*> IgnoreTheseActors;
	IgnoreTheseActors.Add(this);
	
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 10.f, UEngineTypes::ConvertToTraceType(COLLISION_CanClimb), false, IgnoreTheseActors, EDrawDebugTrace::None, HitResult, true))
	{
		ForwardHitLocation = HitResult.Location;
		ForwardHitNormal = HitResult.Normal;
	}

}

void ARepelTheUprisingCharacter::DoVerticalSphereTrace()
{
		FHitResult HitResult;
		const FVector TraceEnd = (GetActorForwardVector() * 75.f) + GetActorLocation();
		FVector TraceStart = TraceEnd;
		TraceStart.Z += 500.f;

		TArray<AActor*> IgnoreTheseActors;
		IgnoreTheseActors.Add(this);
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 10.f, UEngineTypes::ConvertToTraceType(COLLISION_CanClimb), false, IgnoreTheseActors, EDrawDebugTrace::None, HitResult, true))
		{
			HeightLocation = HitResult.Location;
			// Get the location socket found on the player skeleton and find where it is in the vertical
			const FVector ClimbingSocketLocation = GetMesh()->GetSocketLocation(FName("WallClimbingSocket"));
			const float SocketLocationZ = ClimbingSocketLocation.Z;
			// Get the trace hit location in the vertical
			const float OutHitLocZ = HitResult.Location.Z;
			// Only allow the player to hang if the distance is greater than their height plus climbing distance
			if (UKismetMathLibrary::InRange_FloatFloat(SocketLocationZ - OutHitLocZ, MaximumWallClimbDistance, 0.f))
			{
				// Set movement mode to flying to prevent character falling back down
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				GetCharacterMovement()->StopMovementImmediately();
				Hang();
			}
		}

}

void ARepelTheUprisingCharacter::Hang()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (HangMontage && AnimInstance)
	{
		// Play the montage but pause it at the first frame to the player "hangs" in position
		this->PlayAnimMontage(HangMontage, 0.0f, NAME_None);
		AnimInstance->Montage_Pause(HangMontage);
		// Set is climbing to true player cannot turn or move, just look up and down
		bIsClimbing = true;
		// Adjust the position of the player to match the location of the ledge
		FVector LocationToMoveTo;
		const FVector WallNormalAndLocation =  (ForwardHitNormal * (GetCapsuleComponent()->GetUnscaledCapsuleRadius() / 2)) + ForwardHitLocation;
		LocationToMoveTo.X = WallNormalAndLocation.X;
		LocationToMoveTo.Y = WallNormalAndLocation.Y;
		LocationToMoveTo.Z = HeightLocation.Z - 86.f;
		
		// Adjust the rotation of the player to match the rotation of the ledge
		const FRotator RotationToMoveTo = UKismetMathLibrary::MakeRotFromX(ForwardHitNormal * -1.f);
		
		FLatentActionInfo ActionInfo = FLatentActionInfo();
		ActionInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), LocationToMoveTo, RotationToMoveTo, true, true, 0.2f, false, EMoveComponentAction::Move, ActionInfo);

		if (bShouldAutoClimb)
		{
			GetWorldTimerManager().SetTimer(WallClimbTimer, this, &ARepelTheUprisingCharacter::ClimbUp, TimeBeforeAutoClimb, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no valid Hang Montage set."))
	}
}

void ARepelTheUprisingCharacter::TurnAtRate(float Rate)
{
	if (!bIsClimbing)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ARepelTheUprisingCharacter::LookUpAtRate(float Rate)
{
	if (!bIsClimbing)
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ARepelTheUprisingCharacter::ToggleCrouching()
{
	if (!bIsCrouching && !bIsClimbing)
	{
		Crouch();
		bIsCrouching = true;
	}
	else
	{
		UnCrouch();
		bIsCrouching = false;
	}
}

void ARepelTheUprisingCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Only check if we can climb if not already climbing
	if (ClimbCheckTime > 0.f && !bIsClimbing)
	{
		DoForwardSphereTrace();
		DoVerticalSphereTrace();

		ClimbCheckTime -= DeltaSeconds;
	}

}

void ARepelTheUprisingCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ARepelTheUprisingCharacter::OnAnimNotifyBegin);
	}
	
	MaximumWallClimbDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2) * -1.f;
	DefaultWalkSpeed = GetMovementComponent()->GetMaxSpeed();
	MaxRunSpeed = GetMovementComponent()->GetMaxSpeed() * 1.5;
}

void ARepelTheUprisingCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsClimbing)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARepelTheUprisingCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && !bIsClimbing)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ARepelTheUprisingCharacter::StartTurn(float Value)
{
	if (!bIsClimbing)
	{
		AddControllerYawInput(Value);
	}
}

void ARepelTheUprisingCharacter::StartJump()
{
	if (bIsRunning)
	{
		ClimbCheckTime = RunningClimbCheckTime;
	}
	else
	{
		ClimbCheckTime = DefaultClimbCheckTime;		
	}
	
	ACharacter::Jump();
}

void ARepelTheUprisingCharacter::EndJump()
{
	ACharacter::StopJumping();
}

void ARepelTheUprisingCharacter::StartRunning()
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void ARepelTheUprisingCharacter::EndRunning()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

void ARepelTheUprisingCharacter::ClimbUp()
{
	if (bIsClimbing && !bFinishedClimbing)
	{
		bFinishedClimbing = true;
		// Play the montage, letting it run through
		this->PlayAnimMontage(HangMontage, 1.0f, NAME_None);

		// When this montage plays, OnAnimNotifyBegin is then called by the AnimInstance delegate 
	}
}

void ARepelTheUprisingCharacter::OnAnimNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	bIsClimbing = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bFinishedClimbing = false;

	// Move the player forward so it doesn't look like they're hanging in mid-air
	const FVector LocationToMoveTo = (GetCapsuleComponent()->GetComponentLocation() + (GetActorForwardVector() * 60.f));
	
	// Get the current rotation of the player to match
	const FRotator RotationToMoveTo = UKismetMathLibrary::MakeRotFromX(GetActorForwardVector());
		
	FLatentActionInfo ActionInfo = FLatentActionInfo();
	ActionInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), LocationToMoveTo, RotationToMoveTo, true, true, 0.2f, false, EMoveComponentAction::Move, ActionInfo);
}


void ARepelTheUprisingCharacter::DropDown()
{
	// Check to make sure the character is not set to auto climb so drop down is possible and that they are climbing
	if (!bShouldAutoClimb && bIsClimbing)
	{
		bIsClimbing = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		this->StopAnimMontage(HangMontage);
	}
}
