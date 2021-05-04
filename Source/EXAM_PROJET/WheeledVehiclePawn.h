// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WheeledVehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class EXAM_PROJET_API AWheeledVehiclePawn : public AWheeledVehicle
{
	GENERATED_BODY()

public:

	AWheeledVehiclePawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/* Throttle / Steering */
	void ApplySteering(float Value);
	void ApplyThrottle(float Value);

	/* Look Around */
	void LookUp(float Value);
	void Turn(float Value);

	/* Handbrake */
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	/* Update In Air Physics */
	void UpdateInAirControl(float DeltaTime);

protected:

	UPROPERTY(Category=Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	
	UPROPERTY(Category=Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	

	
};
