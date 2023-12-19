// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class AMultiplayerShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	
	friend class AMultiplayerShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool isAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool isAiming);

private:
	AMultiplayerShooterCharacter* Character;

	UPROPERTY(Replicated)
	bool bIsAiming;
	
	UPROPERTY(Replicated)
	AWeapon* EquippedWeapon;
};
