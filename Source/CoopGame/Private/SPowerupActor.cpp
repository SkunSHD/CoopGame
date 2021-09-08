

#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;

	bIsPowerupActive = false;

	SetReplicates(true);
}

void ASPowerupActor::ActivatePowerups()
{
	bIsPowerupActive = true;
	// call for server coz only clients are notified
	OnRep_PowerupStateChange();

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

		bIsPowerupActive = false;
		// call for server coz only clients are notified
		OnRep_PowerupStateChange(); 

		OnExpired();
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	OnPowerupTicked();
}

void ASPowerupActor::OnRep_PowerupStateChange()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}
