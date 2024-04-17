// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCharacterHUD::SetHealthBarPercent(float Health, float MaxHealth)
{
	HealthBar->SetPercent(Health / MaxHealth);
}

void UCharacterHUD::SetDiveCount(int count)
{
	FString DiveText = FString::Printf(TEXT("Dive Count: %d"), count);
	DiveCount->SetText(FText::FromString(DiveText));
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

void UCharacterHUD::SetTimerText(float RemainingTime)
{
	int32 Minutes = FMath::FloorToInt(RemainingTime / 60.f);
	int32 Seconds = RemainingTime - (Minutes * 60);
	
	FString TimerText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	MatchTimerText->SetText(FText::FromString(TimerText));
}

void UCharacterHUD::SetAmmoAmount(int32 Ammo)
{
	FString AmmoText = FString::Printf(TEXT("Ammo: %d"), Ammo);
	if(WeaponAmmoAmount != nullptr)
		WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
}

void UCharacterHUD::SetCarriedAmount(int32 CarriedAmmo)
{
	FString AmmoText = FString::Printf(TEXT("/ %d"), CarriedAmmo);
	CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
}
