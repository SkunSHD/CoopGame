// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "SPowerupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->AddRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Respawn();
	}
}

void ASPickupActor::Respawn()
{
	if (PowerupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no PickupClass for %s"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority() && PowerupInstance)
	{
		PowerupInstance->ActivatePowerups();
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPickup, this, &ASPickupActor::Respawn, CooldownDuration);
	}
}