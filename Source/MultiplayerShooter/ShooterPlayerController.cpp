// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "MultiplayerShooterCharacter.h"

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AMultiplayerShooterCharacter* ShooterCharacter = Cast<AMultiplayerShooterCharacter>(InPawn);
	if(ShooterCharacter)
	{
		ShooterCharacter->SetUIVariables(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}
}
