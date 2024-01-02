// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterHUD::SetHealthBarPercent(float Health, float MaxHealth)
{
	HealthBar->SetPercent(Health / MaxHealth);
}

void UCharacterHUD::SetScoreText(float Score)
{
	FString ScoreText = FString::Printf(TEXT("Score: %d"), FMath::FloorToInt(Score));
	ScoreAmount->SetText(FText::FromString(ScoreText));
}

void UCharacterHUD::SetDeathAmount(int Deaths)
{
	FString DeathText = FString::Printf(TEXT("Deaths: %d"), Deaths);
	DeathAmount->SetText(FText::FromString(DeathText));
}
