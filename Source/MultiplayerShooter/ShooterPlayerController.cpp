// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "MultiplayerShooterCharacter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/GameMode/ShooterGameMode.h"
#include "MultiplayerShooter/GameState/MainGameState.h"
#include "UI/CharacterHUD.h"
#include "UI/ShooterHUD.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/ShooterPlayerState.h"
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
	ShooterGameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if(ShooterGameMode)
	{
		WarmupTime = ShooterGameMode->GetWarmUpTime();
		MatchTime = ShooterGameMode->GetMatchTime();
		LevelStartingTime = ShooterGameMode->GetLevelStartingTime();
		MatchState = ShooterGameMode->GetMatchState();
		CooldownTime = ShooterGameMode->GetCooldownTime();

		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

		if(ShooterHUD && MatchState == MatchState::WaitingToStart)
		{
			ShooterHUD->AddAnnouncement();
		}
	}
}

void AShooterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	CooldownTime = Cooldown;
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
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if(MatchState == MatchState::InProgress)
	{
		HandleHasMatchStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::HandleCooldown()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if(ShooterHUD)
	{
		ShooterHUD->GetCharacterHUD()->RemoveFromParent();
		bool IsHudValid = ShooterHUD->GetAnnouncement() &&
			ShooterHUD->GetAnnouncement()->AnnouncementText &&
			ShooterHUD->GetAnnouncement()->InfoText;
		
		if(IsHudValid)
		{
			ShooterHUD->GetAnnouncement()->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			ShooterHUD->GetAnnouncement()->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
			AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
			FString InfoTextString;
			
			if(MainGameState && ShooterPlayerState)
			{
				TArray<AShooterPlayerState*> TopPlayers = MainGameState->TopScoringPlayers;
				if(TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner...");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == ShooterPlayerState)
				{
					InfoTextString = FString("You Win!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players Tied For Win: \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				ShooterHUD->GetAnnouncement()->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
}

void AShooterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if(MatchState == MatchState::WaitingToStart)
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress)
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown)
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if(HasAuthority())
	{
		ShooterGameMode = ShooterGameMode == nullptr ? Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this)) : ShooterGameMode;
		if(ShooterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(ShooterGameMode->GetCountDownTime() + LevelStartingTime);
		}
	}
	
	if(CountdownInt != SecondsLeft)
	{
		if(MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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
		if(RemainingTime < 0.f)
		{
			ShooterHUD->GetCharacterHUD()->MatchTimerText->SetText(FText());
			return;
		}
		
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
		if(RemainingTime < 0.f)
		{
			ShooterHUD->GetAnnouncement()->WarmupTime->SetText(FText());
			return;
		}
		
		ShooterHUD->GetAnnouncement()->SetWarmupTimeText(RemainingTime);
	}
}
