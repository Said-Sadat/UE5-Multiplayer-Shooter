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
	class UTextBlock* ScoreAmount;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DeathAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchTimerText;

	void SetHealthBarPercent(float Health, float MaxHealth);
	void SetScoreText(float Score);
	void SetDeathAmount(int Deaths);
	void SetTimerText(float RemainingTime);
};
