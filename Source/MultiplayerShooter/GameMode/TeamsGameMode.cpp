// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "MultiplayerShooter/GameState/MainGameState.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/ShooterPlayerController.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PlayerKilled(AMultiplayerShooterCharacter* DeadCharacter,
	AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	Super::PlayerKilled(DeadCharacter, VictimController, AttackerController);

	AMainGameState* ShooterGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;

	if(ShooterGameState && AttackerPlayerState)
	{
		if(AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			ShooterGameState->BlueTeamScores();
		}
		if(AttackerPlayerState->GetTeam() == ETeam::ET_ReadTeam)
		{
			ShooterGameState->RedTeamScores();
		}
	}
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMainGameState* ShooterGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	if(ShooterGameState)
	{
		AShooterPlayerState* ShooterPlayerState = NewPlayer->GetPlayerState<AShooterPlayerState>();
		if(ShooterPlayerState && ShooterPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if(ShooterGameState->BlueTeam.Num() >= ShooterGameState->RedTeam.Num())
			{
				ShooterGameState->RedTeam.AddUnique(ShooterPlayerState);
				ShooterPlayerState->SetTeam(ETeam::ET_ReadTeam);
			}
			else
			{
				ShooterGameState->BlueTeam.AddUnique(ShooterPlayerState);
				ShooterPlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AMainGameState* ShooterGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	AShooterPlayerState* ShooterPlayerState = Exiting->GetPlayerState<AShooterPlayerState>();

	if(ShooterGameState && ShooterPlayerState)
	{
		if(ShooterGameState->RedTeam.Contains(ShooterPlayerState))
		{
			ShooterGameState->RedTeam.Remove(ShooterPlayerState);
		}
		if(ShooterGameState->BlueTeam.Contains(ShooterPlayerState))
		{
			ShooterGameState->BlueTeam.Remove(ShooterPlayerState);
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AShooterPlayerState* AttackerPState = Attacker->GetPlayerState<AShooterPlayerState>();
	AShooterPlayerState* VictimPState = Victim->GetPlayerState<AShooterPlayerState>();

	if(AttackerPState == nullptr || Victim == nullptr) return BaseDamage;
	if(VictimPState == AttackerPState) return BaseDamage;

	if(AttackerPState->GetTeam() == VictimPState->GetTeam())
		return 0;

	return BaseDamage;
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AMainGameState* ShooterGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	if(ShooterGameState)
	{
		for (auto PlayerState : ShooterGameState->PlayerArray)
		{
			AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState.Get());
			if(ShooterPlayerState && ShooterPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if(ShooterGameState->BlueTeam.Num() >= ShooterGameState->RedTeam.Num())
				{
					ShooterGameState->RedTeam.AddUnique(ShooterPlayerState);
					ShooterPlayerState->SetTeam(ETeam::ET_ReadTeam);
				}
				else
				{
					ShooterGameState->BlueTeam.AddUnique(ShooterPlayerState);
					ShooterPlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
