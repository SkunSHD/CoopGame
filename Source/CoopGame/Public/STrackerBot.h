// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USkeletalMeshComponent;
class USHealthComponent;
class UMaterialInstanceDynamic;
class UParticleSystem;
class USphereComponent;
class USoundBase;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent* OuterSphereComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
			const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector FindNextPathPoint();

	FVector NextPathPoint;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	bool bUseVelocityChange;

	// Dynamic material for pulse on damage
	UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	UParticleSystem* ExplosionEffect;

	void SelfDestract();

	bool bExploded;

	bool bStartedSelfDestructionSequence;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float SelfDamageInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	float SelfDestructionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	USoundBase* SelfDestructionExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker Bot")
	USoundBase* StartSelfDestructionSound;

	int32 MaxPowerLevel;

	int32 PowerLevel;

	void OnCheckNearbyBots();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};