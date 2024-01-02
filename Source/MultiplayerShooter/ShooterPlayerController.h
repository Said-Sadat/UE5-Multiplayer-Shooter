// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

	class AShooterHUD* ShooterHUD;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* InPawn) override;

	void SetUIHealth(float Health, float MaxHealth);
};
