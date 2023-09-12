// Copyright 2023 DME Games


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "RepelTheUprising/HealthComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The server and all clients need to be able to see the enemy
	SetReplicates(true);

	RootComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);
	RootComp->InitCapsuleSize(42.f, 96.0f);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	MeshComp->SetupAttachment(RootComp);
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	MovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Movement Comp"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Comp"));

	EnemyStatus = EEnemyStatus::EES_Normal;
	DefaultWalkSpeed = 300.f;
	DefaultRunSpeed = 600.f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComp)
	{
		
	}
	
	MovementComp->MaxWalkSpeed = DefaultWalkSpeed;
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, CurrentMovementSpeed);
}

