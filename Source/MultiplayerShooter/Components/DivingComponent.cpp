// Fill out your copyright notice in the Description page of Project Settings.


#include "DivingComponent.h"

#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UDivingComponent::UDivingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	diveCount = 3;
}

// Called when the game starts
void UDivingComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsDiving && ownerCharacter && ownerCharacter->GetController())
	{
		ServerRPCDiveRotationRequest();

		if(!ownerCharacter->GetMovementComponent()->IsFalling() && CanExitDive)
		{
			bIsDiving = false;

			ownerCharacter->GetController()->SetIgnoreMoveInput(false);
			
			ownerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			ownerCharacter->bUseControllerRotationYaw = true;
		}
	}
}

void UDivingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDivingComponent, bIsDiving);
	DOREPLIFETIME(UDivingComponent, diveDirection);
	DOREPLIFETIME(UDivingComponent, diveRotation);
	DOREPLIFETIME(UDivingComponent, diveCount);
	DOREPLIFETIME(UDivingComponent, CanExitDive);
}

void UDivingComponent::Dive(FVector2D MovementVector)
{
	if(ownerCharacter->GetMovementComponent()->Velocity.Size() == 0) return;
	
	if(bIsDiving || diveCount <= 0) return;

	diveCount -= 1;
	bIsDiving = true;

	ServerRPCDive(MovementVector);
}

void UDivingComponent::ServerRPCDive_Implementation(FVector2D MovementVector)
{
	bIsDiving = true;
	diveDirection = MovementVector;
	MulticastRPCDive(MovementVector);
 }

void UDivingComponent::MulticastRPCDive_Implementation(FVector2D MovementVector)
{
	FVector MovementDirection = ownerCharacter->GetMovementComponent()->Velocity;
	MovementDirection.Normalize();
	MovementDirection.Z = 0.75f;

	ownerCharacter->LaunchCharacter(MovementDirection * 1000, false, false);
	
	bIsDiving = true;
	CanExitDive = false;
	diveDirection = MovementVector;
	
	ownerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	ownerCharacter->bUseControllerRotationYaw = false;

	if(ownerCharacter->GetController())
		ownerCharacter->GetController()->SetIgnoreMoveInput(true);

	ownerCharacter->GetWorldTimerManager().SetTimer(DiveTimer, this, &ThisClass::On_RepCanLeaveDive, 0.3f);
}

void UDivingComponent::ServerRPCDiveRotationRequest_Implementation()
{
	MulticastRPCDiveRotation(ownerCharacter->GetFollowCamera()->GetForwardVector(),
		ownerCharacter->GetMesh()->GetForwardVector());
}

void UDivingComponent::MulticastRPCDiveRotation_Implementation(FVector CameraForward, FVector MeshForward)
{
	diveRotation = GetAngleInDegrees(CameraForward, MeshForward);
}

void UDivingComponent::On_RepCanLeaveDive()
{
	CanExitDive = true;
}

float UDivingComponent::GetAngleInDegrees(FVector VectorA, FVector VectorB)
{
	VectorA.Z = 0;
	VectorB.Z = 0;
	
	float RadiansAngle = FMath::Acos(FVector::DotProduct(VectorA.GetSafeNormal(), VectorB.GetSafeNormal()));
	FVector CrossProduct = FVector::CrossProduct(VectorA, VectorB);

	float Sign = FMath::Sign(CrossProduct.Z);
	float SignedDegreesAngle = FMath::RadiansToDegrees(RadiansAngle) * Sign;

	return SignedDegreesAngle;
}
