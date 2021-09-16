// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	ASGameMode();

protected:

	int32 WaveCount;

	int32 NrOfBotsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void SpawnBotTimerElapsed();

	FTimerHandle TimerHandle_BotSpawned;

	void CheckWaveState();

	FTimerHandle TimerHandle_NextWaveStart;

public:

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

};
