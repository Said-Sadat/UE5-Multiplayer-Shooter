// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "MultiplayerShooterCharacter.h"
#include "UI/CharacterHUD.h"
#include "UI/ShooterHUD.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AMultiplayerShooterCharacter* ShooterCharacter = Cast<AMultiplayerShooterCharacter>(InPawn);
	if(ShooterCharacter)
	{
		SetUIHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}
}

void AShooterPlayerController::SetUIHealth(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetCharacterHUD() &&
			ShooterHUD->GetCharacterHUD()->HealthBar;

	if(isHudValid)
	{
		ShooterHUD->GetCharacterHUD()->SetHealthBarPercent(Health, MaxHealth);
	}
}

void AShooterPlayerController::SetUIScore(float Score)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetCharacterHUD() &&
			ShooterHUD->GetCharacterHUD()->ScoreAmount;

	if(isHudValid)
	{
		ShooterHUD->GetCharacterHUD()->SetScoreText(Score);
	}
}

void AShooterPlayerController::SetDeathCount(float Deaths)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetCharacterHUD() &&
			ShooterHUD->GetCharacterHUD()->DeathAmount;

	if(isHudValid)
	{
		ShooterHUD->GetCharacterHUD()->SetDeathAmount(Deaths);
	}
}
