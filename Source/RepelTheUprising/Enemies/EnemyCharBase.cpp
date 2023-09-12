// Copyright 2023 DME Games


#include "EnemyCharBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "RepelTheUprising/HealthComponent.h"

// Sets default values
AEnemyCharBase::AEnemyCharBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The server and all clients need to be able to see the enemy
	SetReplicates(true);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Comp"));

	EnemyStatus = EEnemyCharStatus::EES_Normal;
	DefaultWalkSpeed = 300.f;
	DefaultRunSpeed = 600.f;

}

// Called when the game starts or when spawned
void AEnemyCharBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

// Called every frame
void AEnemyCharBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCharBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharBase, CurrentMovementSpeed);
}
