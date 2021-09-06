

#include "SPowerupActor.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	ApplyPowerupInterval = 0.0f;
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
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnPowerupTick, true, 0.0f);
	}
	else
	{
		OnPowerupTick();
	}
}

void ASPowerupActor::OnPowerupTick()
{
	PowerupTickProcessed++;

	OnPowerupTicked();

	if (PowerupTickProcessed >= TotalNrOfTicks)
	{
		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

