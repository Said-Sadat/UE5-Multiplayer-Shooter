// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MultiplayerShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMultiplayerShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	FTransform LeftHandTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;

public:
	AMultiplayerShooterCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	void SetOverlappingWeapon(AWeapon* Weapon);
	void PlayFireMontage(bool isAiming);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();

	UFUNCTION(BlueprintCallable)
	bool IsWeaponEquipped();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandTransform();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void FireButtonDown(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void CrouchButtonPressed(const FInputActionValue& Value);
	void AimOffset(float DeltaTime);
			
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	float GetYawOffset();

	void HideCloseCharacter();
	void PlayHitReactMontage();
};