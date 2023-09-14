// Copyright 2023 DME Games


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
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

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Comp"));

	EnemyStatus = EEnemyStatus::EES_Normal;
	TimeBeforeReset = 30.;
	TimeBeforeAlertOff = 30.;
	TimeBeforeCallOffChase = 30.;
}

void AEnemyBase::SetNewStatus(const EEnemyStatus NewStatusIn)
{
	EnemyStatus = NewStatusIn;

	UE_LOG(LogTemp, Warning, TEXT("SetNewStatus called in AEnemyBase"));
	OnStatusChanged();
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::OnStatusChanged()
{
	switch (EnemyStatus)
	{
		case EEnemyStatus::EES_Normal:
			break;
		case EEnemyStatus::EES_Chasing:
			break;
		case EEnemyStatus::EES_Alert:
			break;
		case EEnemyStatus::EES_Combat:
			break;
		case EEnemyStatus::EES_Disabled:
			break;
	
		default: break;
	}
}
