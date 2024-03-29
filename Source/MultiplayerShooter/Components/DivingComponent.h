// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "DivingComponent.generated.h"

class AShooterPlayerController;
class AMultiplayerShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UDivingComponent : public UActorComponent
{
	GENERATED_BODY()

	AShooterPlayerController* OwnerController;
	AMultiplayerShooterCharacter* ownerCharacter;
	FTimerHandle DiveTimer;

	UPROPERTY(Replicated)
	bool bIsDiving;
	UPROPERTY(Replicated)
	FVector2D diveDirection;
	UPROPERTY(Replicated)
	float diveRotation;
	UPROPERTY(Replicated)
	bool CanExitDive;

	UPROPERTY(ReplicatedUsing = OnRep_DiveCount, EditDefaultsOnly, Category = "Dive Stats")
	int diveCount;

public:	
	// Sets default values for this component's properties
	UDivingComponent();
	
	void Dive(FVector2D MovementVector);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE void SetOwnerCharacter(AMultiplayerShooterCharacter* ownerchar) { ownerCharacter = ownerchar; }
	FORCEINLINE void SetOwnerController(AShooterPlayerController* OwnerContr) { OwnerController = OwnerContr; }
	FORCEINLINE void SetIsDiving(bool isDiving) { bIsDiving = isDiving; }

	UFUNCTION(BlueprintCallable)
	void ShouldStartMoving(bool shouldMove);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDiveCount() const { return diveCount; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDiving() const { return bIsDiving; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDiveRotation() const { return diveRotation; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector2D GetDiveDirection() const { return diveDirection; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerRPCDive(FVector2D MovementVector);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDive(FVector2D MovementVector);

	UFUNCTION(Server, Reliable)
	void ServerRPCDiveRotationRequest();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDiveRotation(FVector CameraForward, FVector MeshForward);

private:
	void CanLeaveDive();
	float GetAngleInDegrees(FVector VectorA, FVector VectorB);

	UFUNCTION()
	void OnRep_DiveCount();
};
