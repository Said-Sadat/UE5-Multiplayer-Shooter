// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerState.h"
#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "MultiplayerShooter/ShooterPlayerController.h"
#include "Net/UnrealNetwork.h"

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, Deaths);
	DOREPLIFETIME(AShooterPlayerState, Team);
}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterController = ShooterController == nullptr ? Cast<AShooterPlayerController>(ShooterCharacter->Controller) : ShooterController;
		if(ShooterController)
		{
			ShooterController->SetUIScore(GetScore());
		}
	}
}

void AShooterPlayerState::AddToDeaths(int DeathAmount)
{
	Deaths += DeathAmount;
	
	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterController = ShooterController == nullptr ? Cast<AShooterPlayerController>(ShooterCharacter->Controller) : ShooterController;
		if(ShooterController)
		{
			ShooterController->SetUIDeathCount(Deaths);
		}
	}
}

void AShooterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterCharacter->SetTeamColour(Team);
	}
}

void AShooterPlayerState::OnRep_Team()
{
	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterCharacter->SetTeamColour(Team);
	}
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterController = ShooterController == nullptr ? Cast<AShooterPlayerController>(ShooterCharacter->Controller) : ShooterController;
		if(ShooterController)
		{
			ShooterController->SetUIScore(GetScore());
		}
	}
}

void AShooterPlayerState::OnRep_Deaths()
{
	ShooterCharacter = ShooterCharacter == nullptr ? Cast<AMultiplayerShooterCharacter>(GetPawn()) : ShooterCharacter;
	if(ShooterCharacter)
	{
		ShooterController = ShooterController == nullptr ? Cast<AShooterPlayerController>(ShooterCharacter->Controller) : ShooterController;
		if(ShooterController)
		{
			ShooterController->SetUIDeathCount(Deaths);
		}
	}
}
