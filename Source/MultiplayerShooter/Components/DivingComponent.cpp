// Fill out your copyright notice in the Description page of Project Settings.


#include "DivingComponent.h"

#include "MultiplayerShooter/MultiplayerShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values for this component's properties
UDivingComponent::UDivingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	ownerCharacter = Cast<AMultiplayerShooterCharacter>(GetOwner());
}


// Called when the game starts
void UDivingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UDivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
	if(bIsDiving && ownerCharacter)
	{
		diveDirection = GetAngleInDegrees(ownerCharacter->GetFollowCamera()->GetForwardVector(), ownerCharacter->GetMesh()->GetForwardVector());
		UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), diveDirection);

		if(!ownerCharacter->GetMovementComponent()->IsFalling())
		{
			bIsDiving = false;
			ownerCharacter->GetController()->SetIgnoreMoveInput(false);

			//ownerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			//ownerCharacter->bUseControllerRotationYaw = true;
		}
	}
}

void UDivingComponent::Dive(FVector2D MovementVector)
{
	if(bIsDiving) return;
	
	bIsDiving = true;
	
	FVector MovementDirection = ownerCharacter->GetMovementComponent()->Velocity;
	MovementDirection.Normalize();
	MovementDirection.Z = 1;
	ownerCharacter->LaunchCharacter(MovementDirection * 1000, false, false);
	
	if(MovementVector.X == 1)
		diveDirection = 180;
	if(MovementVector.X == -1)
		diveDirection = 0;
	if(MovementVector.Y == 1)
		diveDirection = 90;
	if(MovementVector.Y == -1)
		diveDirection = -90;

	ownerCharacter->GetController()->SetIgnoreMoveInput(true);
}

void UDivingComponent::SetIsDiving(bool isDiving)
{
	bIsDiving = isDiving;
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




