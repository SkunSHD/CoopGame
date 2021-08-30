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

	SelfDestructionSequenceRate = 0.5f;
	SelfDestructionSequenceRateDamage = 20;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	// get initial path point
	NextPathPoint = FindNextPathPoint();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 4.0f, 0, 1.0f);

	Destroy();
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	

	ASCharacter* Character = Cast<ASCharacter>(OtherActor);
	if (Character)
	{
		if (bStartedSelfDestructionSequence)
		{
			return;
		}
		bStartedSelfDestructionSequence = true;

		GetWorldTimerManager().SetTimer(MyTimerHandle, this, &ASTrackerBot::DamageOnCall, SelfDestructionSequenceRate, true, 0.0f);
	}
}

void ASTrackerBot::DamageOnCall()
{
	UGameplayStatics::ApplyDamage(this, SelfDestructionSequenceRateDamage, GetInstigatorController(), this, nullptr);
}
