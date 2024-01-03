// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "MultiplayerShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/GameMode/ShooterGameMode.h"
#include "UI/CharacterHUD.h"
#include "UI/ShooterHUD.h"
#include "Net/UnrealNetwork.h"
#include "UI/Announcement.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());

	ServerCheckMatchState();
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}

void AShooterPlayerController::CheckTimeSync(float deltaTime)
{
	TimeSyncRunningTime += deltaTime;
	if(IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AShooterPlayerController::ServerCheckMatchState_Implementation()
{
	AShooterGameMode* ShooterGameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if(ShooterGameMode)
	{
		WarmupTime = ShooterGameMode->GetWarmUpTime();
		MatchTime = ShooterGameMode->GetMatchTime();
		LevelStartingTime = ShooterGameMode->GetLevelStartingTime();
		MatchState = ShooterGameMode->GetMatchState();

		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime);

		if(ShooterHUD && MatchState == MatchState::WaitingToStart)
		{
			ShooterHUD->AddAnnouncement();
		}
	}
}

void AShooterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if(ShooterHUD && MatchState == MatchState::WaitingToStart)
	{
		ShooterHUD->AddAnnouncement();
	}
}

void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerController, MatchState);
}

void AShooterPlayerController::OnRep_MatchState()
{
	if(MatchState == MatchState::InProgress)
	{
		HandleHasMatchStarted();
	}
}

void AShooterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if(MatchState == MatchState::WaitingToStart)
	{
		
	}

	if(MatchState == MatchState::InProgress)
	{
		HandleHasMatchStarted();
	}
}

void AShooterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if(MatchState == MatchState::WaitingToStart)
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress)
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	
	if(CountdownInt != SecondsLeft)
	{
		if(MatchState == MatchState::WaitingToStart)
		{
			SetUIAnnouncementTime(TimeLeft);
		}
		if(MatchState == MatchState::InProgress)
		{
			SetUITimer(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
}

void AShooterPlayerController::PollInit()
{
	if(CharacterHUD == nullptr)
	{
		if(ShooterHUD && ShooterHUD->GetCharacterHUD())
		{
			CharacterHUD = ShooterHUD->GetCharacterHUD();
			if(CharacterHUD)
			{
				SetUIHealth(HUDHealth, HUDMaxHealth);
				SetUIScore(HUDScore);
				SetUIDeathCount(HUDDeaths);
			}
		}
	}
}

void AShooterPlayerController::HandleHasMatchStarted()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if(ShooterHUD)
	{
		ShooterHUD->AddCharacterOverlay();
		if(ShooterHUD->GetAnnouncement())
		{
			ShooterHUD->GetAnnouncement()->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AShooterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AShooterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (RoundTripTime / 2);
	ClientServerTimeDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
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

float AShooterPlayerController::GetServerTime()
{
	if(HasAuthority())
		return GetWorld()->GetTimeSeconds();

	return GetWorld()->GetTimeSeconds() + ClientServerTimeDelta;
}

void AShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
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
	else
	{
		InitializeCharacterHUD = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		InitializeCharacterHUD = true;
		HUDScore = Score;
	}
}

void AShooterPlayerController::SetUIDeathCount(float Deaths)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetCharacterHUD() &&
			ShooterHUD->GetCharacterHUD()->DeathAmount;

	if(isHudValid)
	{
		ShooterHUD->GetCharacterHUD()->SetDeathAmount(Deaths);
	}
	else
	{
		InitializeCharacterHUD = true;
		HUDDeaths = Deaths;
	}
}

void AShooterPlayerController::SetUITimer(float RemainingTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetCharacterHUD() &&
			ShooterHUD->GetCharacterHUD()->MatchTimerText;

	if(isHudValid)
	{
		ShooterHUD->GetCharacterHUD()->SetTimerText(RemainingTime);
	}
}

void AShooterPlayerController::SetUIAnnouncementTime(float RemainingTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool isHudValid = ShooterHUD &&
		ShooterHUD->GetAnnouncement() &&
			ShooterHUD->GetAnnouncement()->WarmupTime;

	if(isHudValid)
	{
		ShooterHUD->GetAnnouncement()->SetWarmupTimeText(RemainingTime);
	}
}
