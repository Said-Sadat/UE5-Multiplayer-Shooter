// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

namespace MatchState
{
	extern MULTIPLAYERSHOOTER_API const FName Cooldown; // Match duration has been reached
}

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float CountDownTime = 0.f;
	float LevelStartingTime = 0.f;

protected:
	bool bTeamsMatch = false;
	
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	
public:
	AShooterGameMode();

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	FORCEINLINE bool IsTeamsMatch() const { return bTeamsMatch; }
	FORCEINLINE float GetWarmUpTime() const { return WarmupTime; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetLevelStartingTime() const { return LevelStartingTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }
	FORCEINLINE float GetCountDownTime() const { return CountDownTime; }

	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerKilled(class AMultiplayerShooterCharacter* DeadCharacter, class AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* DeadCharacter, AController* DeadController);
	
};
