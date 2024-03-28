// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

#include "Announcement.h"
#include "CharacterHUD.h"
#include "Blueprint/UserWidget.h"

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();
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

void AShooterHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementClass && Announcement == nullptr)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();
}
