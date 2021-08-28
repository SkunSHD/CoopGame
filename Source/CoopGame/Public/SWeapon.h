// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;
class USoundBase;
class UForceFeedbackEffect;

// contains information of single hitscan weapon line trace
USTRUCT()
struct FHitScanTrance
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactDefaultEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactVulnerableEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	void PlayFireEffects(FVector TracerEndPoint);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	virtual void Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// Derived from RateOfFire
	float TimeBetweenShots;

	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* FireSound;

	//////////////////////////////////////////////////////////////////////////
	// RECOIL SYSTEM

	/**The amount of recoil to apply. We choose a random point from 0-1 on the curve and use it to drive recoil.
	This means designers get lots of control over the recoil pattern*/
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	class UCurveVector* RecoilCurve;

	//The speed at which the recoil bumps up per second
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilSpeed;

	//The speed at which the recoil resets per second
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilResetSpeed;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UPROPERTY(ReplicatedUsing=OnHit_HitScanTrace)
	FHitScanTrance HitScanTrace;

	UFUNCTION()
	void OnHit_HitScanTrace();

public:
	
	void StartFire();

	void StopFire();
	
};
