

#include "SPowerupActor.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::ActivatePowerups()
{
	OnActivated();

	if (TotalNrOfTicks > 0)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnPowerupTick, PowerupInterval, true);
	}
	else
	{
		OnPowerupTick();
	}
}

void ASPowerupActor::OnPowerupTick()
{
	PowerupTickProcessed++;

	if (PowerupTickProcessed >= TotalNrOfTicks)
	{
		UE_LOG(LogTemp, Warning, TEXT("Expired"), *GetName());

		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	OnPowerupTicked();
}

