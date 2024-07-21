// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "LagCompensationComponent.generated.h"

USTRUCT()
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;
	
	UPROPERTY()
	bool bHeadShot;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	AMultiplayerShooterCharacter* Character;
	UPROPERTY()
	class AShooterPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;
	
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, AMultiplayerShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	void CacheBoxPositions(AMultiplayerShooterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AMultiplayerShooterCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AMultiplayerShooterCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AMultiplayerShooterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	
public:	
	ULagCompensationComponent();
	
	friend class AMultiplayerShooterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);
	FServerSideRewindResult ServerSideRewind(class AMultiplayerShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(AMultiplayerShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, class AWeapon* DamageCauser);
};
