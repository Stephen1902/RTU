// Copyright Epic Games, Inc. All Rights Reserved.

#include "RepelTheUprisingCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
	PrimaryActorTick.bCanEverTick = true;
	
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
	BodyMesh->SetLeaderPoseComponent(GetMesh());
	
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands Mesh"));
	HandsMesh->SetupAttachment(GetMesh());
	HandsMesh->SetLeaderPoseComponent(GetMesh());
	
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs Mesh"));
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->SetLeaderPoseComponent(GetMesh());
	
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet Mesh"));
	FeetMesh->SetupAttachment(GetMesh());
	FeetMesh->SetLeaderPoseComponent(GetMesh());
	
	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet Mesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	HelmetMesh->SetLeaderPoseComponent(GetMesh());
	
	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack Mesh"));
	BackpackMesh->SetupAttachment(GetMesh());
	BackpackMesh->SetLeaderPoseComponent(GetMesh());
	
	//////////////////////////////////////////////////////////////////////////
	// Input
}

void ARepelTheUprisingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARepelTheUprisingCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARepelTheUprisingCharacter::EndJump);

		// Move Actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARepelTheUprisingCharacter::Move);
		EnhancedInputComponent->BindAction(RunAction,ETriggerEvent::Triggered, this, &ARepelTheUprisingCharacter::StartRunning);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ARepelTheUprisingCharacter::EndRunning);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARepelTheUprisingCharacter::Look);

		// Climbing
		EnhancedInputComponent->BindAction(ClimbUpAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::ClimbUp);
		EnhancedInputComponent->BindAction(DropDownAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::DropDown);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::ToggleCrouching);
	}
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
		
	SetEnhancedSubsystem();
	GetAnimInst();
	SetDefaultVariables();
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

void ARepelTheUprisingCharacter::SetEnhancedSubsystem() const
{
	// Try to get the controlled player and set their Enhanced Input settings
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ARepelTheUprisingCharacter::GetAnimInst()
{
	// Get the animation assigned to this player
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ARepelTheUprisingCharacter::OnAnimNotifyBegin);
	}
}

void ARepelTheUprisingCharacter::SetDefaultVariables()
{
	MaximumWallClimbDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2) * -1.f;
	DefaultWalkSpeed = GetMovementComponent()->GetMaxSpeed();
	MaxRunSpeed = GetMovementComponent()->GetMaxSpeed() * MaxSpeedMultiplier;
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

void ARepelTheUprisingCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARepelTheUprisingCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller.  There is a modifier in the editor but this doesn't work with the X Axis which needs reversing
		AddControllerYawInput(LookAxisVector.X * -1.f);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
