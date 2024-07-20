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

public:	
	ULagCompensationComponent();
	
	friend class AMultiplayerShooterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);
	void ServerSideRewind(class AMultiplayerShooterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);
};
