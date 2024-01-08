// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 100000.f

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	bool IsFireButtonPressed;
	float DefaultFOV;
	float CurrentFOV;
	bool CanFire = true;
	FTimerHandle FireTimer;
	FHitResult HitTarget;

	class AMultiplayerShooterCharacter* Character;

	UPROPERTY(Replicated)
	bool bIsAiming;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	UPROPERTY(EditAnywhere, Category= "Combat")
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category= "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);
	void StartFireTimer();
	void FireTimerFinish();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool isAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	
public:	
	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SetAiming(bool isAiming);
	void FireButtonPressed(bool isPressed);
};
