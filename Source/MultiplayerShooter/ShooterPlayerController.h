// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

	class AShooterHUD* ShooterHUD;
	class UCharacterHUD* CharacterHUD;
	class AShooterGameMode* ShooterGameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;
	bool InitializeCharacterHUD = false;
	float HUDHealth, HUDMaxHealth, HUDScore, HUDDeaths, HUDDiveCount;
	bool InitializeDiveCount = false;
	float HUDCarriedAmmo;
	bool InitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool InitializeWeaponAmmo = false;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void SetHUDTime();
	void PollInit();
	void HandleHasMatchStarted(bool bTeamsMatch = false);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerTimeDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();

	void CheckTimeSync(float deltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void SetUIHealth(float Health, float MaxHealth);
	void SetUIDiveCount(int diveCount);
	void SetUIScore(float Score);
	void SetUIDeathCount(float Deaths);
	void SetUIWeaponAmmo(int32 Ammo);
	void SetUICarriedAmmo(int32 CarriedAmmo);
	void SetUITimer(float RemainingTime);
	void SetUIAnnouncementTime(float RemainingTime);
	void HideTeamScores();
	void InitTeamScores();
	void SetUIRedTeamScore(int32 RedScore);
	void SetUIBlueTeamScore(int32 BlueScore);
	
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleCooldown();
};
