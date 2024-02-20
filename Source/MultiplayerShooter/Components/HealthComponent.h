// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category= "Health")
	float MaxHealth;
	UPROPERTY(ReplicatedUsing= OnRep_Health, VisibleAnywhere, Category= "Health")
	float Health;

	UFUNCTION()
	void OnRep_Health();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UHealthComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage();
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float damage);
	
};
