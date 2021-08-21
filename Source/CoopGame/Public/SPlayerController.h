// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void SetupInputComponent() override;

	void ApplyRecoil(const FVector2D& RecoilAmount, const float RecoilSpeed, const float RecoilResetSpeed);

protected:

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FVector2D RecoilBumpAmount;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FVector2D RecoilResetAmount;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	float CurrentRecoilSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	float CurrentRecoilResetSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	float LastRecoilTime;

	void LookUp(float Rate);

	void Turn(float Rate);

};
