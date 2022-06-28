// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"


enum class EWaveState : uint8;


UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	ASGameMode();

protected:

	int32 WaveCount;

	int32 NrOfBotsToSpawn;

	FTimerHandle TimerHandle_BotSpawned;

	FTimerHandle TimerHandle_NextWaveStart;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void SpawnBotTimerElapsed();

	void CheckWaveState();

	void CheckGameOverState();

	bool HasAnyBotAlive();

	bool HasAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewWaveState);

public:

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

};
