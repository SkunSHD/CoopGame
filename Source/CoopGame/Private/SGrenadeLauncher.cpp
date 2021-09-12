// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"
#include "Components/SphereComponent.h"

void ASGrenadeLauncher::Fire()
{
	//UE_LOG(LogTemp, Warning, TEXT("GeneratedBP"));
	
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No projectile has been set"));
		return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		if (!HasAuthority())
		{
			ServerFire();
		}

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);

		PlayFireEffects(FVector());

		LastFireTime = GetWorld()->TimeSeconds;
	}
}