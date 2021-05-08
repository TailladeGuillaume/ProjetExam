// Fill out your copyright notice in the Description page of Project Settings.


#include "WheeledVehiclePawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

static const FName NAME_SteerInput_Air("SteerAir");
static const FName NAME_ThrottleInput_Air("ThrottleAir");



AWheeledVehiclePawn::AWheeledVehiclePawn()
{
    UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

    // Adjust the Tire Loading
    Vehicle4W->MinNormalizedTireLoad = 0.0f;
    Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
    Vehicle4W->MaxNormalizedTireLoad = 2.0f;
    Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

    //Torque Setup
    Vehicle4W->MaxEngineRPM = 5700.0f;
    Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset(); 
    Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
    Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
    Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

    //Adjust Steering
    Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
    Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
    Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
    Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

    Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
    Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

    //Automatic Gearbox
    Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
    Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
    Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.f;

    //Create the SpringArmComponent
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 250.0f;
    SpringArm->bUsePawnControlRotation = true;

    //Create The Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->FieldOfView = 90.0f;
}

void AWheeledVehiclePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateInAirControl(DeltaTime);
}

void AWheeledVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &AWheeledVehiclePawn::ApplyThrottle);
    PlayerInputComponent->BindAxis(NAME_SteerInput, this, &AWheeledVehiclePawn::ApplySteering);

    PlayerInputComponent->BindAxis(NAME_ThrottleInput_Air);
    PlayerInputComponent->BindAxis(NAME_SteerInput_Air);
    
    PlayerInputComponent->BindAxis("MouseLookUpRate", this, &AWheeledVehiclePawn::LookUp);
    PlayerInputComponent->BindAxis("MouseTurnRate", this, &AWheeledVehiclePawn::Turn);

    PlayerInputComponent->BindAxis("BaseLookUpRate", this, &AWheeledVehiclePawn::LookUp);
    PlayerInputComponent->BindAxis("BaseTurnRate", this, &AWheeledVehiclePawn::Turn);

    PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AWheeledVehiclePawn::OnHandbrakePressed);
    PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AWheeledVehiclePawn::OnHandbrakeReleased);


}

void AWheeledVehiclePawn::ApplySteering(float Value)
{
    GetVehicleMovementComponent()->SetSteeringInput(Value);
}

void AWheeledVehiclePawn::ApplyThrottle(float Value)
{
    GetVehicleMovementComponent()->SetThrottleInput(Value);
}

void AWheeledVehiclePawn::LookUp(float Value)
{
    if(Value != 0.0f)
    {
        AddControllerPitchInput(Value);
    }
}

void AWheeledVehiclePawn::Turn(float Value)
{
    if(Value != 0.0f)
    {
        AddControllerYawInput(Value);
    }
}

void AWheeledVehiclePawn::OnHandbrakePressed()
{
    GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AWheeledVehiclePawn::OnHandbrakeReleased()
{
    GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AWheeledVehiclePawn::UpdateInAirControl(float DeltaTime)
{
    if(UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement()))
    {
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);

        const FVector TraceStart = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
        const FVector TraceEnd = GetActorLocation() - FVector(0.0f, 0.0f, 200.0f);

        FHitResult Hit;

        //Check if car it's flipped on its side, and check if the car is in the air
        const bool bInAir = !GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
        const bool bNotGrounded = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < 0.1f;

        //Only allow in-air movement if we are not on the ground, or are in the air
        if(bInAir || bNotGrounded)
        {
			
			if(IsValid(InputComponent))
			{
			    const float ForwardInput = InputComponent->GetAxisValue(NAME_ThrottleInput_Air);
			    const float RightInput = InputComponent->GetAxisValue(NAME_SteerInput_Air);
			

			    UE_LOG(LogTemp, Warning, TEXT("ForwardInput : %f / RightInput : %f"), ForwardInput, RightInput);

			    //In car is grounded allow player to roll the car over
			    const float AirMovementForcePitch = 3.0f;
			    const float AirMovementForceRoll = !bInAir && bNotGrounded ? 10.0f : 3.0f;

			    if(UPrimitiveComponent* VehicleMesh = Vehicle4W->UpdatedPrimitive)
			    {
			        const FVector MovementVector = FVector(RightInput * -AirMovementForceRoll, ForwardInput * AirMovementForcePitch, 0.0f) * DeltaTime * 150.0f;
			        const FVector NewAngularMovement = GetActorRotation().RotateVector(MovementVector);

			        VehicleMesh->SetPhysicsAngularVelocity(NewAngularMovement, true);
			    }
			}
        }
    }
}
