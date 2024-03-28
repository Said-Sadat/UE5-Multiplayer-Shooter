// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UCharacterHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DiveCount;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreAmount;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DeathAmount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchTimerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	void SetHealthBarPercent(float Health, float MaxHealth);
	void SetDiveCount(int count);
	void SetScoreText(float Score);
	void SetDeathAmount(int Deaths);
	void SetTimerText(float RemainingTime);
	void SetAmmoAmount(int32 Ammo);
	void SetCarriedAmount(int32 CarriedAmmo);
};
