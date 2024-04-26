// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatState.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"

#include "CombatComponent.generated.h"

class AShooterPlayerController;
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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapon();
	void Reload();
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	bool ShouldSwapWeapon();
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool isAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool isAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	void Fire();
	void FireButtonPressed(bool isPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerRPCReload();

	void AttachActorToRightHand(AWeapon* WeaponToEquip);
	void AttachActorToBackPack(AWeapon* WeaponToEquip);
	void DropEquippedWeapon();
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	void HandleReload();
	int32 AmountToReload();
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	

private:
	AMultiplayerShooterCharacter* Character;
	AShooterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
	
	UPROPERTY(Replicated)
	bool bIsAiming;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool IsFireButtonPressed;
	float DefaultFOV;
	float CurrentFOV;
	FHitResult HitTarget;

	UPROPERTY(EditAnywhere, Category= "Combat")
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category= "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	FTimerHandle FireTimer;
	bool bCanFire = true;

	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 15;
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 1;
	
	void InitializeCarriedAmmo();
	void UpdateAmmoValues();
	void StartFireTimer();
	void FireTimerFinish();
	bool CanFire();
};
