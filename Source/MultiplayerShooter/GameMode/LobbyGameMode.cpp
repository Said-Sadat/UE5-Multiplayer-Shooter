// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerSessionsSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);
	}

	if(GameState)
	{
		int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumOfPlayers)
				);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if(PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();

				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)
					);
			}

			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players required: %d"), MinPlayers)
				);
		}

		if(NumOfPlayers == MinPlayers)
		{
			UWorld* World = GetWorld();
			if(World)
			{
				bUseSeamlessTravel = true;
				
				if(Subsystem)
					MatchType = Subsystem->GetDesiredMatchType();
				
				if(MatchType == "Teams")
					World->ServerTravel(FString("/Game/Blueprints/Maps/Map4?listen"));
				else
					World->ServerTravel(FString("/Game/Blueprints/Maps/Map3?listen"));
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(GameState)
	{
		int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumOfPlayers -1)
				);

			APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
			if(PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();

				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has exited the game!"), *PlayerName)
					);
			}
		}
	}
}
