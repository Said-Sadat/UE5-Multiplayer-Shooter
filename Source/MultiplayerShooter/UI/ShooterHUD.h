// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	class UCharacterHUD* CharacterHUD;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<class UUserWidget> AnnouncementClass;
	class UAnnouncement* Announcement;

protected:
	virtual void BeginPlay() override;
	
public:
	void AddCharacterOverlay();
	void AddAnnouncement();
	
	virtual void DrawHUD() override;

	FORCEINLINE UCharacterHUD* GetCharacterHUD() const { return CharacterHUD; }
	FORCEINLINE UAnnouncement* GetAnnouncement() const { return Announcement; }
};
