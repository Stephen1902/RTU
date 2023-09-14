// Copyright 2023 DME Games


#include "EnemyCharBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AEnemyCharBase::AEnemyCharBase()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	MeshComp->SetupAttachment(GetRootComponent());
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	MovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Movement Comp"));
}

void AEnemyCharBase::SetNewStatus(EEnemyStatus NewStatusIn)
{
	Super::SetNewStatus(NewStatusIn);
	
}

void AEnemyCharBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharBase, CurrentMovementSpeed);
}

void AEnemyCharBase::OnStatusChanged()
{
	Super::OnStatusChanged();

	
}


