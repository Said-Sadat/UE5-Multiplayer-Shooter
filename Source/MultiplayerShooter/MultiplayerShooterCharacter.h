// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Team.h"
#include "GameFramework/Character.h"
#include "Combat/CombatState.h"
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
	bool IsInputSetup = false;

	UPROPERTY(Replicated)
	FVector2D MovementVector;

	class AShooterPlayerController* ShooterPlayerController;
	class AShooterPlayerState* ShooterPlayerState;
	class AShooterGameMode* ShooterGameMode;
	void UpdateHUDHealth();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;
	bool IsDead = false;

	FTimerHandle DeadTimer;
	UPROPERTY(EditDefaultsOnly)
	float DeadDelay = 3.f;
	
	UFUNCTION()
	void OnRep_Health();

	void DeadTimerFinished();

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
	UInputAction* DiveAction;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;
	UPROPERTY(VisibleAnywhere)
	class UDivingComponent* DivingComponent;
	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category= Combat, meta=(AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Teams)
	USkeletalMesh* RedSkeletalMesh;

	UPROPERTY(EditAnywhere, Category = Teams)
	USkeletalMesh* BlueSkeletalMesh;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	UFUNCTION(BlueprintCallable)
	float GetYawOffset();

	void HideCloseCharacter();
	void PlayHitReactMontage();

protected:
	void Move(const FInputActionValue& Value);
	void Dive(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void FireButtonDown(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void CrouchButtonPressed(const FInputActionValue& Value);
	void AimOffset(float DeltaTime);
	void Reload();
	void PollInit();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
			
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

public:
	AMultiplayerShooterCharacter();

	// Hit Boxes:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calve_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calve_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

	UPROPERTY()
	TMap<FName, UBoxComponent*> HitCollisionBoxes;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UDivingComponent* GetDivingComponent() const { return DivingComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }
	FORCEINLINE ULagCompensationComponent* GetLagCompoensationComponent() const { return LagCompensation; }
	
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsDead() const { return IsDead; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector2D GetMovementVector() const { return MovementVector; }

	ECombatState GetCombatState() const;

	void SpawnDefaultWeapon();
	void SetOverlappingWeapon(AWeapon* Weapon);
	void PlayFireMontage(bool isAiming);
	void PlayReloadMontage();
	void PlayDeathMontage();
	void UpdateUIAmmo();
	void Dead();
	void SetTeamColour(ETeam Team);
	bool IsLocallyReloading();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
	UFUNCTION(BlueprintCallable)
	bool UseFABRIK();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastDead();
	UFUNCTION(BlueprintCallable)
	bool IsWeaponEquipped();
	UFUNCTION(BlueprintCallable)
	bool GetIsAiming();
	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandTransform();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

};