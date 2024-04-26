// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "MultiplayerShooter/ShooterPlayerController.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "MultiplayerShooter/GameState/MainGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShooterPlayerController* ShooterPlayer = Cast<AShooterPlayerController>(*It);
		if(ShooterPlayer)
		{
			ShooterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

void AShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if(CountDownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if(MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

float AShooterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AShooterGameMode::PlayerKilled(AMultiplayerShooterCharacter* DeadCharacter, AShooterPlayerController* VictimController,
                                    AShooterPlayerController* AttackerController)
{
	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;

	AMainGameState* MainGameState = GetGameState<AMainGameState>();

	bool ShouldAddToScore = AttackerPlayerState &&
		AttackerPlayerState != VictimPlayerState &&
			MainGameState &&
				MatchState == MatchState::InProgress;
	
	if(ShouldAddToScore)
	{
		AttackerPlayerState->AddToScore(1.0f);
		MainGameState->UpdateTopScore(AttackerPlayerState);
	}

	if(VictimPlayerState)
	{
		VictimPlayerState->AddToDeaths(1);
	}
	
	if(DeadCharacter)
	{
		DeadCharacter->Dead();
	}
}

void AShooterGameMode::RequestRespawn(ACharacter* DeadCharacter, AController* DeadController)
{
	if(DeadCharacter)
	{
		DeadCharacter->Reset();
		DeadCharacter->Destroy();
	}
	if(DeadController)
	{
		TArray<AActor*> PlayerStartActors;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartActors);
		int32 RandomSelection = FMath::RandRange(0, PlayerStartActors.Num() -1);
		RestartPlayerAtPlayerStart(DeadController, PlayerStartActors[RandomSelection]);
	}
}
