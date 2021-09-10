// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBarrel.generated.h"

class USHealthComponent;
class UStaticMeshComponent;
class EploudedMaterial;
class UParticleSystem;
class URadialForceComponent;

UCLASS()
class COOPGAME_API ASBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
			const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_bExploded();

	UPROPERTY(ReplicatedUsing=OnRep_bExploded)
	bool bExploded;

	void PlayExplosionEffects();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(EditDefaultsOnly, Category = "Barrel")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Barrel")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ExplosionForce;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UMaterial* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ExplosionEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
