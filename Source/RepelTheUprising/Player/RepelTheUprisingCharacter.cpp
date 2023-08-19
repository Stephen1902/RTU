// Copyright Epic Games, Inc. All Rights Reserved.

#include "RepelTheUprisingCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerInGameWidget.h"
#include "RepelTheUprisingPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RepelTheUprising/HealthComponent.h"
#include "RepelTheUprising/StaminaComponent.h"
#include "RepelTheUprising/FoodComponent.h"
#include "RepelTheUprising/WaterComponent.h"
#include "RepelTheUprising/InventoryComponent.h"
#include "RepelTheUprising/Interactive/InteractionComponent.h"
#include "Net/UnrealNetwork.h"

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

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));
	FoodComponent = CreateDefaultSubobject<UFoodComponent>(TEXT("Food Component"));
	WaterComponent = CreateDefaultSubobject<UWaterComponent>(TEXT("Water Component"));
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->SetCapacity(100);
	InventoryComponent->SetWeightCapacity(200.);
	
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

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARepelTheUprisingCharacter::Look);

		// Climbing
		EnhancedInputComponent->BindAction(ClimbUpAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::ClimbUp);
		EnhancedInputComponent->BindAction(DropDownAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::DropDown);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::ToggleCrouching);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARepelTheUprisingCharacter::BeginSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARepelTheUprisingCharacter::EndSprint);
	}
}

void ARepelTheUprisingCharacter::BeginSprint(const FInputActionValue& Value)
{
	// Check that this character has stamina to use
	if (StaminaComponent && !FMath::IsNearlyZero(StaminaComponent->GetCurrentStamina()))
	{
		ModifySprintStatus(true);	
	}
}

void ARepelTheUprisingCharacter::EndSprint(const FInputActionValue& Value)
{
	ModifySprintStatus(false);
}

void ARepelTheUprisingCharacter::ModifySprintStatus(bool bNewSprintStatus)
{
	if (!HasAuthority())
	{
		Server_ModifySprintStatus(bNewSprintStatus);
	}
	else
	{
		if (StaminaComponent)
		{
			StaminaComponent->SetStaminaShouldDrain(bNewSprintStatus);
		}
	}
}

void ARepelTheUprisingCharacter::OnStaminaChange(float CurrentStaminaIn)
{
	// If this character has run out of stamina, stop them being able to do the things stamina requires
	if (FMath::IsNearlyZero(CurrentStaminaIn))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has run out of stamina"), *GetName());
		ModifySprintStatus(false);
	}
}

void ARepelTheUprisingCharacter::Server_ModifySprintStatus_Implementation(bool bNewSprintStatus)
{
	ModifySprintStatus(bNewSprintStatus);
}

void ARepelTheUprisingCharacter::DoForwardSphereTrace()
{
	if (!HasAuthority())
	{
		Server_DoForwardSphereTrace();
	}

	FHitResult HitResult;
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd = (GetActorForwardVector() * 150.0) + TraceStart;
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
	if (!HasAuthority())
	{
		Server_DoVerticalSphereTrace();
	}
	
	FHitResult HitResult;
	const FVector TraceEnd = (GetActorForwardVector() * 75.0) + GetActorLocation();
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
			// Set is climbing to true player cannot turn or move, just look up and down
			bIsClimbing = true;
			Server_Hang();
		}
	}
}

void ARepelTheUprisingCharacter::ClimbUp()
{
	// This function is only called if the player can "hang" on a ledge and will need to be called on the server if a client
	if (HasAuthority())
	{
		Multicast_ClimbUp(); 
	}
	else
	{
		Server_ClimbUp();
	}
}

void ARepelTheUprisingCharacter::Multicast_ClimbUp_Implementation()
{
	if (bIsClimbing && !bFinishedClimbing)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is inside the ClimbUp Implementation"), *GetName());
		bFinishedClimbing = true;
		// Play the montage, letting it run through
		this->PlayAnimMontage(HangMontage, 1.f, NAME_None);
		
		// When this montage plays, OnAnimNotifyBegin is then called by the AnimInstance delegate 
	}
}

void ARepelTheUprisingCharacter::Hang_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (HangMontage && AnimInstance)
	{
		// Play the montage but pause it at the first frame to the player "hangs" in position
		this->PlayAnimMontage(HangMontage, 0.0f, NAME_None);
		AnimInstance->Montage_Pause(HangMontage);
		// Adjust the position of the player to match the location of the ledge
		const FVector WallNormalAndLocation =  (ForwardHitNormal * (GetCapsuleComponent()->GetUnscaledCapsuleRadius() / 2)) + ForwardHitLocation;
		FVector LocationToMoveTo;
		LocationToMoveTo.X = WallNormalAndLocation.X;
		LocationToMoveTo.Y = WallNormalAndLocation.Y;
		LocationToMoveTo.Z = HeightLocation.Z - 86.f;
		
		// Adjust the rotation of the player to match the rotation of the ledge
		const FRotator RotationToMoveTo = UKismetMathLibrary::MakeRotFromX(ForwardHitNormal * -1.0);
		
		FLatentActionInfo ActionInfo = FLatentActionInfo();
		ActionInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), LocationToMoveTo, RotationToMoveTo, true, true, 0.2f, false, EMoveComponentAction::Move, ActionInfo);

		// Tell the server to climb up the ledge
		if (bShouldAutoClimb)
		{
			GetWorldTimerManager().SetTimer(WallClimbTimer, this, &ARepelTheUprisingCharacter::Server_ClimbUp, TimeBeforeAutoClimb, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no valid Hang Montage set."))
	}
}

void ARepelTheUprisingCharacter::ToggleCrouching()
{
	// Check to make sure we're not in an action blocking menu
	if (!bIsInMenu)
	{
		// The player cannot crouch if they are climbing 
		if (!bIsClimbing)
		{
			// Toggle the crouch action
			if (!bIsCrouching)
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
	}
}

void ARepelTheUprisingCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Check to make sure we're not in an action blocking menu
	if(!bIsInMenu)
	{
		// Only check if we can climb if not already climbing
		if (ClimbCheckTime > 0.f && !bIsClimbing)
		{
			Server_DoForwardSphereTrace();
			Server_DoVerticalSphereTrace();

			ClimbCheckTime -= DeltaSeconds;
		}
		else
		{
			// Check whether player needs to look for an interactable component
			TimeSinceLastInteraction -= DeltaSeconds;
			//  Check whether the time between interactions has passed, check for an interactive component if it has
			if (TimeSinceLastInteraction <= 0.f )
			{
				// Reset the time to check for a new interaction
				TimeSinceLastInteraction = TimeBetweenInteractionChecks;
				DoInteractionCheck();
			}
		}
	}
}

void ARepelTheUprisingCharacter::BeginPlay()
{
	Super::BeginPlay();
		
	SetEnhancedSubsystem();
	// Set the variables that need to be used by the server
	SetDefaultVariables();

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &ARepelTheUprisingCharacter::OnHealthIsChanged);
	}

	if (StaminaComponent)
	{
		StaminaComponent->OnStaminaChanged.AddDynamic(this, &ARepelTheUprisingCharacter::OnStaminaChange);
	}

	if (ARepelTheUprisingPlayerController* PC = Cast<ARepelTheUprisingPlayerController>(GetController()))
	{
		PC->OnInventoryStatusChanged.AddDynamic(this, &ARepelTheUprisingCharacter::GetIsInMenuStatus);
	}

	CreatePlayerWidgets();
	bIsInMenu = false;
}

void ARepelTheUprisingCharacter::StartJump()
{
	if (!bIsInMenu)
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
}

void ARepelTheUprisingCharacter::EndJump()
{
	ACharacter::StopJumping();
}

bool ARepelTheUprisingCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ARepelTheUprisingCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ARepelTheUprisingCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ARepelTheUprisingCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ARepelTheUprisingCharacter::OnAnimNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is in AnimNotify"), *GetName());
	
	// Move the player forward so it doesn't look like they're hanging in mid-air
	const FVector LocationToMoveTo = (GetCapsuleComponent()->GetComponentLocation() + (GetActorForwardVector() * 60.0));
	
	// Get the current rotation of the player to match
	const FRotator RotationToMoveTo = UKismetMathLibrary::MakeRotFromX(GetActorForwardVector());
		
	FLatentActionInfo ActionInfo = FLatentActionInfo();
	ActionInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), LocationToMoveTo, RotationToMoveTo, true, true, 0.2f, false, EMoveComponentAction::Move, ActionInfo);

	bIsClimbing = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bFinishedClimbing = false;
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

void ARepelTheUprisingCharacter::SetDefaultVariables()
{
	if (!HasAuthority())
	{
		Server_SetMovementVariables();
	}

	// Get the animation assigned to this player
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ARepelTheUprisingCharacter::OnAnimNotifyBegin);
	}
	
	MaximumWallClimbDistance = (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2) * -1.f;
	DefaultWalkSpeed = GetMovementComponent()->GetMaxSpeed();
	MaxRunSpeed = GetMovementComponent()->GetMaxSpeed() * MaxSpeedMultiplier;
}

void ARepelTheUprisingCharacter::DoInteractionCheck()
{
	// Get the location and rotation of where the player is looking
	const FVector TraceStart = CameraComp->GetComponentLocation();
	const FVector TraceEnd = (CameraComp->GetForwardVector() * LineTraceDistance) + TraceStart;
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	// Make sure we don't check for ourselves
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		//Check if we hit an InteractionComp object
		if (HitResult.GetActor())
		{
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(HitResult.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - HitResult.ImpactPoint).Size();
				// Check to make sure that the distance the player is away from the interaction component is within where the IC can be interacted with
				if (InteractionComponent != GetInteractionComp() && Distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractionComp(InteractionComponent);
				}
				else if (Distance > InteractionComponent->InteractionDistance && GetInteractionComp())
				{
					CouldntFindInteractionComp();
				}
				return;
			}
		}
	}
	// If not interaction component is found, clear the old one out
	CouldntFindInteractionComp();
}

void ARepelTheUprisingCharacter::CouldntFindInteractionComp()
{
	// Player has lost focus on an interaction component. Clear the timer.
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}
	
	//Tell the interaction component we've stopped focusing on it, and clear it.
	if (UInteractionComponent* InteractionComp = GetInteractionComp())
	{
		InteractionComp->EndFocus(this);

		if (InteractionData.bIsInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void ARepelTheUprisingCharacter::FoundNewInteractionComp(TObjectPtr<UInteractionComponent> InteractionCompIn)
{
	// Clear the old interaction component, if there is one
	EndInteract();

	if (UInteractionComponent* OldInteractionComp = GetInteractionComp())
	{
		OldInteractionComp->EndFocus(this);
	}

	InteractionData.ViewedInteractionComponent = InteractionCompIn;
	InteractionCompIn->BeginFocus(this);
}

void ARepelTheUprisingCharacter::BeginInteract()
{
	if (!HasAuthority())
	{
		ServerBeginInteract();
	}

	/**As an optimization, the server only checks that we're looking at an item once we begin interacting with it.
	This saves the server doing a check every tick for an InteractionComp Item. The exception is a non-instant interact.
	In this case, the server will check every tick for the duration of the interact*/
	if (HasAuthority())
	{
		DoInteractionCheck();
	}

	InteractionData.bIsInteractHeld = true;

	if (UInteractionComponent* InteractionComp = GetInteractionComp())
	{
		InteractionComp->BeginInteract(this);

		if (FMath::IsNearlyZero(InteractionComp->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ARepelTheUprisingCharacter::Interact, InteractionComp->InteractionTime, false);
		}
	}
}

void ARepelTheUprisingCharacter::EndInteract()
{
	if (!HasAuthority())
	{
		ServerEndInteract();
	}
	
	InteractionData.bIsInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* InteractionComp = GetInteractionComp())
	{
		InteractionComp->EndInteract(this);
	}
}

void ARepelTheUprisingCharacter::Server_Hang_Implementation()
{
	Hang();
}

void ARepelTheUprisingCharacter::Server_DoForwardSphereTrace_Implementation()
{
	DoForwardSphereTrace();
}

void ARepelTheUprisingCharacter::Server_DoVerticalSphereTrace_Implementation()
{
	DoVerticalSphereTrace();
}

void ARepelTheUprisingCharacter::Server_ClimbUp_Implementation()
{
	ClimbUp();
}

void ARepelTheUprisingCharacter::CreatePlayerWidgets()
{
	if (MainWidget)
	{	
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			MainWidgetRef = CreateWidget<UPlayerInGameWidget>(PC, MainWidget);
			MainWidgetRef->SetPlayerCharacterRef(this);
			MainWidgetRef->AddToViewport();
		}
	}
}

void ARepelTheUprisingCharacter::GetIsInMenuStatus(const bool bNewMenuStatus)
{
	bIsInMenu = bNewMenuStatus;
}

void ARepelTheUprisingCharacter::Server_SetMovementVariables_Implementation()
{
	SetDefaultVariables();
}

void ARepelTheUprisingCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ARepelTheUprisingCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ARepelTheUprisingCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* InteractionComp = GetInteractionComp())
	{
		InteractionComp->Interact(this);
	}
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
	if (!bIsInMenu)
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
}

void ARepelTheUprisingCharacter::Look(const FInputActionValue& Value)
{
	if (!bIsInMenu)
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
}

void ARepelTheUprisingCharacter::OnHealthIsChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	
}


void ARepelTheUprisingCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARepelTheUprisingCharacter, bIsRunning);
	DOREPLIFETIME(ARepelTheUprisingCharacter, bIsClimbing);
	DOREPLIFETIME(ARepelTheUprisingCharacter, ForwardHitNormal);
	DOREPLIFETIME(ARepelTheUprisingCharacter, ForwardHitLocation);
	DOREPLIFETIME(ARepelTheUprisingCharacter, HeightLocation);
	DOREPLIFETIME(ARepelTheUprisingCharacter, bFinishedClimbing);
	DOREPLIFETIME(ARepelTheUprisingCharacter, MaximumWallClimbDistance);
}

