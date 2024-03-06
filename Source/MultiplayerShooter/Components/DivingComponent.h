// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DivingComponent.generated.h"

class AMultiplayerShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UDivingComponent : public UActorComponent
{
	GENERATED_BODY()

	AMultiplayerShooterCharacter* ownerCharacter;

	UPROPERTY(Replicated)
	FVector2D diveDirection;
	UPROPERTY(Replicated)
	float diveRotation;
	UPROPERTY(Replicated)
	bool bIsDiving;

public:	
	// Sets default values for this component's properties
	UDivingComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Dive(FVector2D MovementVector);

	FORCEINLINE void SetOwnerCharacter(AMultiplayerShooterCharacter* ownerchar) { ownerCharacter = ownerchar; }
	FORCEINLINE void SetIsDiving(bool isDiving) { bIsDiving = isDiving; }

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
	void ServerDive(FVector2D MovementVector);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastServerDive(FVector2D MovementVector);
private:
	float GetAngleInDegrees(FVector VectorA, FVector VectorB);
};
