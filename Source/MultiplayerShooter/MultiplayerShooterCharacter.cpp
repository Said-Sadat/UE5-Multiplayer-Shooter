// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerShooterCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Combat/CombatComponent.h"
#include "MultiplayerShooter.h"
#include "ShooterPlayerController.h"
#include "GameMode/ShooterGameMode.h"
#include "TimerManager.h"
#include "Combat/CombatComponent.h"
#include "Components/DivingComponent.h"
#include "Components/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/WeaponTypes.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerShooterCharacter

AMultiplayerShooterCharacter::AMultiplayerShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	bReplicates = true;
	
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Combat->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("Lag Compensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	DivingComponent = CreateDefaultSubobject<UDivingComponent>(TEXT("Diving Component"));
	DivingComponent->SetIsReplicated(true);

	// Hit box set-up.
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	calve_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calve_l"));
	calve_l->SetupAttachment(GetMesh(), FName("calve_l"));
	calve_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	calve_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calve_r"));
	calve_r->SetupAttachment(GetMesh(), FName("calve_r"));
	calve_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMultiplayerShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMultiplayerShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AMultiplayerShooterCharacter, bIsAiming);
	DOREPLIFETIME(AMultiplayerShooterCharacter, Health);
	DOREPLIFETIME(AMultiplayerShooterCharacter, MovementVector);
}

void AMultiplayerShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(Combat)
		Combat->Character = this;

	if(LagCompensation)
	{
		LagCompensation->Character = this;
		if(Controller)
			LagCompensation->Controller = Cast<AShooterPlayerController>(Controller);
	}

	if(DivingComponent)
	{
		DivingComponent->SetOwnerCharacter(this);
		DivingComponent->SetOwnerController(Cast<AShooterPlayerController>(GetController()));
	}
}

void AMultiplayerShooterCharacter::SetTeamColour(ETeam Team)
{
	if(GetMesh() == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_ReadTeam:
		if(RedSkeletalMesh)
			GetMesh()->SetSkeletalMesh(RedSkeletalMesh);
		break;
	case ETeam::ET_BlueTeam:
		if(BlueSkeletalMesh)
			GetMesh()->SetSkeletalMesh(BlueSkeletalMesh);
		break;
	case ETeam::ET_NoTeam:
		if(BlueSkeletalMesh)
			GetMesh()->SetSkeletalMesh(BlueSkeletalMesh);
		break;
	}
}

bool AMultiplayerShooterCharacter::IsLocallyReloading()
{
	if(Combat == nullptr) return false;
	return Combat->GetIsLocallyReloading();
}

void AMultiplayerShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	ShooterPlayerController = Cast<AShooterPlayerController>(Controller);
	
	if (ShooterPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ShooterPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			IsInputSetup = true;
		}
	}

	if(HasAuthority())
	{
		SpawnDefaultWeapon();
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	UpdateUIAmmo();
	UpdateHUDHealth();
}

void AMultiplayerShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority() && !IsInputSetup && Controller)
	{
		ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
		if(ShooterPlayerController)
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ShooterPlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				IsInputSetup = true;
			}	
		}
	}

	AimOffset(DeltaSeconds);
	HideCloseCharacter();
	PollInit();

}

void AMultiplayerShooterCharacter::PollInit()
{

	if(ShooterPlayerController)
		if(ShooterPlayerController->GetMatchState() != MatchState::InProgress) return;
	
	if(ShooterPlayerState == nullptr)
	{
		ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
		if(ShooterPlayerState)
		{
			ShooterPlayerState->AddToScore(0);
			ShooterPlayerState->AddToDeaths(0);
			SetTeamColour(ShooterPlayerState->GetTeam());
		}
	}
	
	
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if(ShooterPlayerController)
	{
		UpdateUIAmmo();
		UpdateHUDHealth();
	}
}

// Input

void AMultiplayerShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerShooterCharacter::Move);
		EnhancedInputComponent->BindAction(DiveAction, ETriggerEvent::Triggered, this, &ThisClass::Dive);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerShooterCharacter::Look);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Aim);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireButtonDown);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireButtonReleased);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ThisClass::Equip);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::CrouchButtonPressed);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::Reload);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiplayerShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerShooterCharacter::Dive(const FInputActionValue& Value)
{
	if(DivingComponent && Combat->EquippedWeapon)
		DivingComponent->Dive(MovementVector);
}

void AMultiplayerShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMultiplayerShooterCharacter::Aim(const FInputActionValue& Value)
{
	if(Combat)
	{
		if(GetMovementComponent()->IsFalling()) return;
		
		Combat->SetAiming(Value.Get<bool>());
	}
}

void AMultiplayerShooterCharacter::FireButtonDown(const FInputActionValue& Value)
{
	if(Combat)
	{
		Combat->FireButtonPressed(Value.Get<bool>());
	}
}

void AMultiplayerShooterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AMultiplayerShooterCharacter::Equip(const FInputActionValue& Value)
{
	if(Combat)
	{
		ServerEquipButtonPressed();
	}
}

void AMultiplayerShooterCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AMultiplayerShooterCharacter::AimOffset(float DeltaTime)
{
	if(Combat && Combat->EquippedWeapon == nullptr) return;
	
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	float Speed = Velocity.Size();
	bool isInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.f && !isInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		//bUseControllerRotationYaw = true;
	}

	if(Speed > 0.f || isInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		//bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2d InRange(270.f, 360.f);
		FVector2d OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AMultiplayerShooterCharacter::Reload()
{
	if(Combat)
		Combat->Reload();
}

void AMultiplayerShooterCharacter::UpdateUIAmmo()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if(ShooterPlayerController && Combat && Combat->EquippedWeapon)
	{
		ShooterPlayerController->SetUIWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
		ShooterPlayerController->SetUICarriedAmmo(Combat->CarriedAmmo);
	}
}

void AMultiplayerShooterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                 AController* InstigatorController, AActor* DamageCauser)
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	Damage = ShooterGameMode->CalculateDamage(InstigatorController, Controller, Damage);
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	UpdateHUDHealth();
	PlayHitReactMontage();

	if(Health == 0.f)
	{
		if(ShooterGameMode)
		{
			AShooterPlayerController* ReceiverController = Cast<AShooterPlayerController>(Controller);
			AShooterPlayerController* AttackerController = Cast<AShooterPlayerController>(InstigatorController);
			
			ShooterGameMode->PlayerKilled(this, ReceiverController, AttackerController);
		}
	}
}

ECombatState AMultiplayerShooterCharacter::GetCombatState() const
{
	if(Combat)
		return Combat->CombatState;

	return ECombatState::ECS_MAX;
}

bool AMultiplayerShooterCharacter::UseFABRIK()
{
	return GetCombatState() != ECombatState::ECS_Reloading;
}

void AMultiplayerShooterCharacter::SpawnDefaultWeapon()
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	UWorld* World = GetWorld();
	if(ShooterGameMode && World && !IsDead && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		if(Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AMultiplayerShooterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	
	OverlappingWeapon = Weapon;

	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AMultiplayerShooterCharacter::PlayFireMontage(bool isAiming)
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bIsAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	
}

void AMultiplayerShooterCharacter::PlayReloadMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Sniper");
			break;
		}
		
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMultiplayerShooterCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
}

void AMultiplayerShooterCharacter::Dead()
{
	if(Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	
	MultiCastDead();
	
	GetWorldTimerManager().SetTimer(
		DeadTimer,
		this,
		&ThisClass::DeadTimerFinished,
		DeadDelay);
}

void AMultiplayerShooterCharacter::MultiCastDead_Implementation()
{
	if(ShooterPlayerController)
	{
		ShooterPlayerController->SetUIWeaponAmmo(0);
	}
	IsDead = true;
	PlayDeathMontage();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if(AShooterPlayerController* ShooterController = Cast<AShooterPlayerController>(Controller))
	{
		DisableInput(ShooterController);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool HideSniperScope = IsLocallyControlled() &&
		Combat &&
		Combat->bIsAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	
	if(HideSniperScope)
		ShowSniperScopeWidget(false);
}

void AMultiplayerShooterCharacter::UpdateHUDHealth()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? ShooterPlayerController = Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;

	if(ShooterPlayerController)
		ShooterPlayerController->SetUIHealth(Health, MaxHealth);
}

void AMultiplayerShooterCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void AMultiplayerShooterCharacter::DeadTimerFinished()
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	if(ShooterGameMode)
	{
		ShooterGameMode->RequestRespawn(this, Controller);
	}
}

void AMultiplayerShooterCharacter::PlayHitReactMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		SectionName = FName("Front");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

bool AMultiplayerShooterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AMultiplayerShooterCharacter::GetIsAiming()
{
	return Combat && Combat->bIsAiming;
}

FTransform AMultiplayerShooterCharacter::GetLeftHandTransform()
{
	if(Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	{
		LeftHandTransform = Combat->EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		return LeftHandTransform;
	}

	return FTransform();
}

void AMultiplayerShooterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

float AMultiplayerShooterCharacter::GetYawOffset()
{
	FRotator AimRotation = GetBaseAimRotation();
	FRotator MoveRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());

	return UKismetMathLibrary::NormalizedDeltaRotator(MoveRotation, AimRotation).Yaw;
}

void AMultiplayerShooterCharacter::HideCloseCharacter()
{
	if(!IsLocallyControlled()) return;

	float distance = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size();
	
	if(distance < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);

		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AMultiplayerShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(Combat)
	{
		if(OverlappingWeapon)
			Combat->EquipWeapon(OverlappingWeapon);
		else
		{
			if(Combat->ShouldSwapWeapon())
			{
				Combat->SwapWeapon();
			}
		}
	}
}

