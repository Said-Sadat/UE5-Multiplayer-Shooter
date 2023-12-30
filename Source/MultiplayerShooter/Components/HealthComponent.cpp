// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

float UHealthComponent::GetHealthPercentage()
{
	return Health / MaxHealth;
}

void UHealthComponent::TakeDamage(float damage)
{
	if(Health >= 0)
	{
		Health -= damage;
	}

	if(Health < 0)
	{
		Health = 0;

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Character Dead"))
				);
		}
	}
}

void UHealthComponent::OnRep_Health()
{
}
