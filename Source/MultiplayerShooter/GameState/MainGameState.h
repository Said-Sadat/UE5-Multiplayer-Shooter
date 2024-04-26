// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMainGameState : public AGameState
{
	GENERATED_BODY()

	float TopScore = 0.f;

public:
	UPROPERTY(Replicated)
	TArray<class AShooterPlayerState*> TopScoringPlayers;

	TArray<AShooterPlayerState*> RedTeam;
	TArray<AShooterPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_RedTeamScore();
	UFUNCTION()
	void OnRep_BlueTeamScore();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateTopScore(AShooterPlayerState* ScoringPlayer);
};
