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

	if(bIsDiving && ownerCharacter)
	{
		ServerRPCDiveRotationRequest();

		if(!ownerCharacter->GetMovementComponent()->IsFalling())
		{
			bIsDiving = false;

			if(ownerCharacter->GetController())
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
}

void UDivingComponent::Dive(FVector2D MovementVector)
{
	if(bIsDiving) return;
	
	ServerRPCDive(MovementVector);
}

void UDivingComponent::ServerRPCDive_Implementation(FVector2D MovementVector)
{
	MulticastRPCDive(MovementVector);
}

void UDivingComponent::MulticastRPCDive_Implementation(FVector2D MovementVector)
{
	bIsDiving = true;
	diveDirection = MovementVector;

	ownerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	ownerCharacter->bUseControllerRotationYaw = false;
    
	FVector MovementDirection = ownerCharacter->GetMovementComponent()->Velocity;
	MovementDirection.Normalize();
	MovementDirection.Z = 1;
    
	ownerCharacter->LaunchCharacter(MovementDirection * 1000, false, false);

	if(ownerCharacter->GetController())
		ownerCharacter->GetController()->SetIgnoreMoveInput(true);
}

void UDivingComponent::ServerRPCDiveRotationRequest_Implementation()
{
	diveRotation = GetAngleInDegrees(ownerCharacter->GetFollowCamera()->GetForwardVector(),
			ownerCharacter->GetMesh()->GetForwardVector());
	
	MulticastRPCDiveRotation(diveRotation);
}

void UDivingComponent::MulticastRPCDiveRotation_Implementation(float diverotate)
{
	diveRotation = diverotate;
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
