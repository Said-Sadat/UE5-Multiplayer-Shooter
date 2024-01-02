// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"

#include "Components/ProgressBar.h"

void UCharacterHUD::SetHealthBarPercent(float Health, float MaxHealth)
{
	HealthBar->SetPercent(Health / MaxHealth);
}
