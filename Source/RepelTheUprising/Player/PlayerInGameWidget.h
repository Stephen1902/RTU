// Copyright 2023 DME Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class REPELTHEUPRISING_API UPlayerInGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayerCharacterRef(class ARepelTheUprisingCharacter* PlayerRefIn);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")	
	TObjectPtr<ARepelTheUprisingCharacter> PlayerCharacterRef;

};