// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"
#include "SGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	MaxHealth = 100;
	
	TeamNum = 255;

	SetIsReplicated(true);
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();


	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = MaxHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Called only on the server
	if (bInvulnerable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invulnerable mode"));
		return;
	}
	if (Damage < 0 || bIsDead)
	{
		return;
	}

	if (DamageCauser != DamagedActor && IsFriendly(DamageCauser, DamagedActor))
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health: %f %s"), Health, *GetName());

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), InstigatedBy);
		}
	}
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}

// broadcast for clients
void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = OldHealth - Health;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}
	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Health: %f (+%f)"), Health, HealAmount);

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
	{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}
	
	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));
	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}
	
	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}
