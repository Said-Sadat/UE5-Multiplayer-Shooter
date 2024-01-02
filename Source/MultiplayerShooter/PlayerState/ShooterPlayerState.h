// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

	class AMultiplayerShooterCharacter* ShooterCharacter;
	class AShooterPlayerController* ShooterController;

	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int Deaths;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Deaths();
	void AddToScore(float ScoreAmount);
	void AddToDeaths(int DeathAmount);
};
