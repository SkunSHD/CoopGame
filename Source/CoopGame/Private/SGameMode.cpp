// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "Components/SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"


ASGameMode::ASGameMode()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	TimeBetweenWaves = 2.0f;
}


// TickInterval is changed
void ASGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckWaveState();

	CheckGameOverState();
}


void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}


void ASGameMode::CheckWaveState()
{
	bool bPreparing = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);
	if (bPreparing || NrOfBotsToSpawn > 0)
	{
		return;
	}

	if (!HasAnyBotAlive())
	{
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}
}


void ASGameMode::CheckGameOverState()
{
	if (!HasAnyPlayerAlive())
	{
		GameOver();
	}
}


bool ASGameMode::HasAnyBotAlive()
{
	bool bIsAnyBotAlive = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = It->Get();
		if (Pawn == nullptr || Pawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	return bIsAnyBotAlive;
}


bool ASGameMode::HasAnyPlayerAlive()
{
	bool bIsAnyPlayerAlive = false;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) {
			continue;
		}

		APawn* Pawn = PC->GetPawn();
		if (!Pawn) {
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyPlayerAlive = true;
			break;
		}
	}

	return bIsAnyPlayerAlive;
}


void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawned, this, &ASGameMode::SpawnBotTimerElapsed, TimeBetweenWaves, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}


void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawned);

	SetWaveState(EWaveState::WaitingToComplete);
}


void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}


void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false, 0.0f);

	SetWaveState(EWaveState::WaitingToStart);

	RespawnDeadPlayers();
}


void ASGameMode::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Over!"));

	SetWaveState(EWaveState::GameOver);
}


void ASGameMode::SetWaveState(EWaveState NewWaveState)
{
	ASGameState* GS = GetGameState<ASGameState>();

	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewWaveState);
	}
}

void ASGameMode::RespawnDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr) {
			RestartPlayer(PC);
		}
	}
}