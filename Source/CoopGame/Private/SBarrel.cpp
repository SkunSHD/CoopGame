// Fill out your copyright notice in the Description page of Project Settings.


#include "SBarrel.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASBarrel::ASBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	SetRootComponent(MeshComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ASBarrel::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASBarrel::OnHealthChanged);

}

// Called every frame
void ASBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bExploded)
	{
		// Explosion
		bExploded = true;

		// Change material
		if (ExplodedMaterial)
		{
			MeshComp->SetMaterial(0, ExplodedMaterial);
		}

		// Launch upwards
		FVector BarrelImpulse = MeshComp->GetMass() * MeshComp->GetUpVector() * ExplosionForce;
		MeshComp->AddImpulse(BarrelImpulse, NAME_None, true);

		// Play Effects
		if (ExplosionEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, MeshComp->GetComponentLocation());
		}

		RadialForceComp->FireImpulse();

		UE_LOG(LogTemp, Warning, TEXT("Exploded !!!"));
	}
}

