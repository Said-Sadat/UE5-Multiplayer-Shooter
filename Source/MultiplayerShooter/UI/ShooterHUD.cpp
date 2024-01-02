// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

#include "CharacterHUD.h"
#include "Blueprint/UserWidget.h"

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AShooterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterHUD = CreateWidget<UCharacterHUD>(PlayerController, CharacterOverlayClass);
		CharacterHUD->AddToViewport();
	}
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
}
