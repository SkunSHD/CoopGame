// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "SCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	// Overlap Comp
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	// Health
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	RequiredDistanceToTarget = 100;
	MovementForce = 1000;

	bUseVelocityChange = true;

	ExplosionDamage = 40;
	ExplosionRadius = 200;

	SelfDamageInterval = 0.25f;
	SelfDestructionDamage = 20;

	MaxPowerLevel = 4;
	PowerLevel = 0;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	// get initial path point
	if (HasAuthority())
	{
		NextPathPoint = FindNextPathPoint();
		
		FTimerHandle TimerHandle_CheckNearbyBots;
		GetWorldTimerManager().SetTimer(TimerHandle_CheckNearbyBots, this, &ASTrackerBot::OnCheckNearbyBots, 1.0f, true, 0.0f);
	}
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bExploded)
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			// target reached
			NextPathPoint = FindNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "GOTCHA!");
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.f, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
	}
}

void ASTrackerBot::OnCheckNearbyBots()
{
	const float Radius = 600;

	// temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::White, false, 1.0f);

	int32 NumOfBots = 0;
	for (FOverlapResult Result : Overlaps)
	{
		ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());
		if (Bot && Bot != this)
		{
			NumOfBots++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("NumOfBots: %d"), NumOfBots);

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		PowerLevel = FMath::Clamp(NumOfBots, 0, MaxPowerLevel);

		float Alpha = PowerLevel / (float)MaxPowerLevel;
		UE_LOG(LogTemp, Warning, TEXT("Alpha: %f"), Alpha);

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 0), FString::FromInt(PowerLevel), this, FColor::White, true);
}

FVector ASTrackerBot::FindNextPathPoint()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	// failed to find the next point
	return GetActorLocation();
}

void ASTrackerBot::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHealthChanged %f, %s"), Health, *GetName());

	// Pulse on hit
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0)
	{
		SelfDestract();
	}
}

void ASTrackerBot::SelfDestract()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	UGameplayStatics::SpawnSoundAtLocation(this, SelfDestructionExplosionSound, GetActorLocation());
	
	// Hide it as if it was destroyed until SetLifeSpan works
	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);
		UE_LOG(LogTemp, Warning, TEXT("PowerLevel %d"), PowerLevel);
		UE_LOG(LogTemp, Warning, TEXT("ExplosionDamage %f"), ExplosionDamage);
		UE_LOG(LogTemp, Warning, TEXT("DamageWithBonus %f"), ActualDamage);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 4.0f, 0, 1.0f);

		SetLifeSpan(1.0f);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartedSelfDestructionSequence)
	{
		return;
	}
	
	ASCharacter* Character = Cast<ASCharacter>(OtherActor);
	if (Character)
	{
		bStartedSelfDestructionSequence = true;

		if (HasAuthority())
		{
			FTimerHandle TimerHandle_SelfDamage;
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
		}

		UGameplayStatics::SpawnSoundAttached(StartSelfDestructionSound, RootComponent);
	}
}

void ASTrackerBot::DamageSelf()
{
	if (bExploded)
	{
		return;
	}
	UGameplayStatics::ApplyDamage(this, SelfDestructionDamage, GetInstigatorController(), this, nullptr);
}
