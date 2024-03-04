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

	float diveDirection;
	bool bIsDiving;

public:	
	// Sets default values for this component's properties
	UDivingComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Dive(FVector2D MovementVector);
	void SetIsDiving(bool isDiving);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDiving() const { return bIsDiving; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDiveDirection() const { return diveDirection; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	float GetAngleInDegrees(FVector VectorA, FVector VectorB);
};
