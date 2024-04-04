// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float DamageInnerRadius = 200.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float DamageOuterRadius = 500.f;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	AProjectileRocket();
};
