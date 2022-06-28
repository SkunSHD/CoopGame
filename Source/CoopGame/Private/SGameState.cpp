// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"



void ASGameState::OnRep_WaveState(EWaveState OldWaveState)
{
	WaveStateChanged(WaveState, OldWaveState);
}


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}


void ASGameState::SetWaveState(EWaveState NewWaveState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		const EWaveState OldWaveState = WaveState;
		WaveState = NewWaveState;
		
		// Call on server manually coz OnRep is called automatically only on clients
		OnRep_WaveState(OldWaveState);
	}
}
