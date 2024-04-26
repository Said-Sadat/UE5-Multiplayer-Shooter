// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"

#include "MultiplayerShooter/ShooterPlayerController.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainGameState, TopScoringPlayers);
	DOREPLIFETIME(AMainGameState, RedTeamScore);
	DOREPLIFETIME(AMainGameState, BlueTeamScore);
}

void AMainGameState::UpdateTopScore(AShooterPlayerState* ScoringPlayer)
{
	if(TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AMainGameState::RedTeamScores()
{
	RedTeamScore += 1;

	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController());
	if(ShooterPlayerController)
		ShooterPlayerController->SetUIRedTeamScore(RedTeamScore);
}

void AMainGameState::BlueTeamScores()
{
	BlueTeamScore += 1;

	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController());
	if(ShooterPlayerController)
		ShooterPlayerController->SetUIBlueTeamScore(BlueTeamScore);
}

void AMainGameState::OnRep_RedTeamScore()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController());
	if(ShooterPlayerController)
		ShooterPlayerController->SetUIRedTeamScore(RedTeamScore);
}

void AMainGameState::OnRep_BlueTeamScore()
{
	AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController());
	if(ShooterPlayerController)
		ShooterPlayerController->SetUIBlueTeamScore(BlueTeamScore);
}