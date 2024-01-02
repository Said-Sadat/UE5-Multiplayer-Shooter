// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "MultiplayerShooter/ShooterPlayerController.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"

void AShooterGameMode::PlayerKilled(AMultiplayerShooterCharacter* DeadCharacter, AShooterPlayerController* VictimController,
	AShooterPlayerController* AttackerController)
{
	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;

	if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
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
