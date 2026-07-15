// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalCharacterMovementComponent.h"

#include "Coastal.h"
#include "CoastalCharacter.h"
#include "CoastalEquipmentMeshComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

const float UCoastalCharacterMovementComponent::BRAKE_TO_STOP_VELOCITY_SQUARED = BRAKE_TO_STOP_VELOCITY
                                                                                 * BRAKE_TO_STOP_VELOCITY;

#pragma region FSavedMove

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::Clear()
{
    FSavedMove_Character::Clear();

    // reset flags
    Saved_bWantsToSprint = 0u;
    Saved_PreviousMovementMode = MOVE_None;
    Saved_PreviousCustomMovementMode = CMOVE_None;
}

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::SetMoveFor(ACharacter* C, float InDeltaTime,
                                                                        FVector const& NewAccel,
                                                                        FNetworkPredictionData_Client_Character& ClientData)
{
    FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(C->GetCharacterMovement());

    Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
    Saved_PreviousMovementMode = CharacterMovement->Safe_PreviousMovementMode;
    Saved_PreviousCustomMovementMode = CharacterMovement->Safe_PreviousCustomMovementMode;
}

bool UCoastalCharacterMovementComponent::FSavedMove_Coastal::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                            ACharacter* InCharacter, float MaxDelta) const
{
    FSavedMove_Coastal* NewCoastalMove = static_cast<FSavedMove_Coastal*>(NewMove.Get());

    if (Saved_bWantsToSprint != NewCoastalMove->Saved_bWantsToSprint)
    {
        return false;
    }
    if (Saved_PreviousMovementMode != NewCoastalMove->Saved_PreviousMovementMode)
    {
        return false;
    }
    if (Saved_PreviousCustomMovementMode != NewCoastalMove->Saved_PreviousCustomMovementMode)
    {
        return false;
    }

    // defer to super class
    return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::PrepMoveFor(ACharacter* C)
{
    FSavedMove_Character::PrepMoveFor(C);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(C->GetCharacterMovement());

    CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
    CharacterMovement->Safe_PreviousMovementMode = Saved_PreviousMovementMode;
    CharacterMovement->Safe_PreviousCustomMovementMode = Saved_PreviousCustomMovementMode;
}

uint8 UCoastalCharacterMovementComponent::FSavedMove_Coastal::GetCompressedFlags() const
{
    uint8 Result = FSavedMove_Character::GetCompressedFlags();

    if (Saved_bWantsToSprint)
    {
        Result |= FLAG_Custom_0;
    }

    return Result;
}

#pragma endregion

#pragma region FNetworkPredictionData_Client

UCoastalCharacterMovementComponent::FNetworkPredictionData_Client_Coastal::FNetworkPredictionData_Client_Coastal(
    const UCharacterMovementComponent& ClientMovement)
    : FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr UCoastalCharacterMovementComponent::FNetworkPredictionData_Client_Coastal::AllocateNewMove()
{
    return MakeShared<FSavedMove_Coastal>();
}

#pragma endregion

#pragma region UCharacterMovementComponent

UCoastalCharacterMovementComponent::UCoastalCharacterMovementComponent()
{
    BrakingFrictionFactor = 1.f;  // true drag

    DefaultCustomMovementMode = CMOVE_Skate;
}

FNetworkPredictionData_Client* UCoastalCharacterMovementComponent::GetPredictionData_Client() const
{
    check(PawnOwner != nullptr);
    if (ClientPredictionData == nullptr)
    {
        UCoastalCharacterMovementComponent* MutableThis = const_cast<UCoastalCharacterMovementComponent*>(this);

        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Coastal(*this);
    }
    return ClientPredictionData;
}

void UCoastalCharacterMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    CoastalCharacterOwner = Cast<ACoastalCharacter>(GetOwner());
}

void UCoastalCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    Safe_bWantsToSprint = static_cast<bool>(Flags & FSavedMove_Character::FLAG_Custom_0);
}

void UCoastalCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    Safe_PreviousMovementMode = PreviousMovementMode;
    Safe_PreviousCustomMovementMode = static_cast<ECustomMovementMode>(PreviousCustomMode);

    if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Skate)
    {
        ExitSkate();
    }
    else if (IsCustomMovementMode(CMOVE_Skate))
    {
        EnterSkate();
    }
}

void UCoastalCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
    Super::PhysCustom(DeltaTime, Iterations);

    switch (CustomMovementMode)
    {
        case CMOVE_Skate:
            PhysSkate(DeltaTime, Iterations);
            break;
        default:
            UE_LOG(LogCoastal, Fatal, TEXT("Invalid Movement Mode"));
            break;
    }
}

bool UCoastalCharacterMovementComponent::CanAttemptJump() const
{
    return Super::CanAttemptJump() || (IsCustomMovementMode(CMOVE_Skate) && GetHitNormalCharacterEquipment().has_value());
}

float UCoastalCharacterMovementComponent::GetMaxSpeed() const
{
    // override max speed with custom sprint mode
    if (MovementMode == MOVE_Walking)
    {
        return Safe_bWantsToSprint ? MaxSpeedSprintWalking : MaxWalkSpeed;
    }

    if (MovementMode != MOVE_Custom)
    {
        return Super::GetMaxSpeed();
    }

    switch (CustomMovementMode)
    {
        case CMOVE_Skate:
            return Safe_bWantsToSprint ? MaxSpeedSprintSkating : MaxSpeedSkating;
        default:
            UE_LOG(LogTemp, Error, TEXT("Invalid Movement Mode"))
            return -1.f;
    }
}

float UCoastalCharacterMovementComponent::GetMaxAcceleration() const
{
    if (MovementMode != MOVE_Custom)
    {
        return Super::GetMaxAcceleration();
    }

    switch (CustomMovementMode)
    {
        case CMOVE_Skate:
            return MaxAccelerationSkating;
        default:
            UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
            return -1.f;
    }
}

float UCoastalCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
    if (MovementMode != MOVE_Custom)
    {
        return Super::GetMaxBrakingDeceleration();
    }

    switch (CustomMovementMode)
    {
        case CMOVE_Skate:
            return BrakingDecelerationSkating;
        default:
            UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
            return -1.f;
    }
}

void UCoastalCharacterMovementComponent::SetDefaultMovementMode()
{
    Super::SetDefaultMovementMode();

    if (DefaultLandMovementMode == MOVE_Custom)
    {
        SetMovementMode(MOVE_Custom, DefaultCustomMovementMode);
    }
}

void UCoastalCharacterMovementComponent::EnterSkate() const
{
    CoastalCharacterOwner->GetEquipmentMeshComponent()->SetVisibility(true);
}

void UCoastalCharacterMovementComponent::ExitSkate()
{
    // TODO: move visibiility to animation blueprint
    if (MovementMode != MOVE_Falling)
    {
        CoastalCharacterOwner->GetEquipmentMeshComponent()->SetVisibility(false);
    }

    FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector)
                            .ToQuat();
    FHitResult HitResult;
    SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, HitResult);
}

void UCoastalCharacterMovementComponent::PhysSkate(float DeltaTime, int32 Iterations)
{
    // proceed only on tick
    if (DeltaTime < MIN_TICK_TIME)
    {
        return;
    }

    // proceed only if in a valid state
    if (!CharacterOwner
        || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion()
            && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
    {
        Velocity = FVector::ZeroVector;
        Acceleration = FVector::ZeroVector;
        return;
    }

    // loop for time-stepped physics within a single tick
    float RemainingTime = DeltaTime;
    while ((RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner
           && (CharacterOwner->Controller || bRunPhysicsWithNoController
               || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
    {
        Iterations++;
        bJustTeleported = false;
        const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
        RemainingTime -= TimeTick;

        // update velocity as a function of gravity. update velocity rather than acceleration to keep acceleration reserved for user input
        Velocity += GetGravityZ() * FVector::UpVector * TimeTick;  // v = v + at
        CalcVelocity(TimeTick, FrictionSkating, false, GetMaxBrakingDeceleration());

        // compute displacement during this tick
        const FVector Displacement = TimeTick * Velocity;  // dx = v * dt
        if (Displacement.IsNearlyZero())
        {
            RemainingTime = 0.f;  // make sure this is the last iteration
        }

        // compute up vector as either the current up vector or the new hit normal
        FVector Up = UpdatedComponent->GetUpVector();
        if (std::optional<FVector> OptionHitNormal = GetHitNormalCharacterEquipment(); OptionHitNormal.has_value())
        {
            // if hit normal did not change enough, keep the hit normal as the current up vector
            if (FVector HitNormal = OptionHitNormal.value(); !HitNormal.Cross(Up).IsNearlyZero(0.001f))
            {
                Up = HitNormal;
            }
        }
        else
        {
            // start falling
            SetMovementMode(MOVE_Falling);
            StartNewPhysics(RemainingTime, Iterations);
            return;
        }

        // do not use the z-component when computing whether we should substitute forward vector for velocity
        const FVector Velocity2D = FVector(Velocity.X, Velocity.Y, 0.f);
        const FVector Forward = Velocity2D.SizeSquared() < BRAKE_TO_STOP_VELOCITY_SQUARED ? UpdatedComponent->GetForwardVector()
                                                                                          : Velocity;
        // adjust forward vector onto the desired plane by removing component that points along up
        const FVector ProjectedForward = FVector::VectorPlaneProject(Forward, Up);

        // compute rotation from desired forward and desired up
        const FQuat Rotation = ProjectedForward.IsNearlyZero()
                                   ? UpdatedComponent->GetComponentQuat()
                                   : FRotationMatrix::MakeFromXZ(ProjectedForward.GetSafeNormal(), Up).ToQuat();

        // save location before movement
        const FVector OldLocation = UpdatedComponent->GetComponentLocation();

        // perform the actual movement
        FHitResult SafeMoveHitResult(1.f);
        SafeMoveUpdatedComponent(Displacement, Rotation, true, SafeMoveHitResult);

        // check for if anything was hit during movement
        if (SafeMoveHitResult.Time < 1.f)
        {
            HandleImpact(SafeMoveHitResult, TimeTick, Displacement);
            SlideAlongSurface(Displacement, 1.f - SafeMoveHitResult.Time, SafeMoveHitResult.Normal, SafeMoveHitResult, true);
        }

        // adjust velocity & acceleration in case of impact during safe movement
        if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
        {
            Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / TimeTick;  // v = dx / dt
        }
    }
}

std::optional<FVector> UCoastalCharacterMovementComponent::GetHitNormalCharacter() const
{
    FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start
                  + CoastalCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;

    static const FName PROFILE_NAME = TEXT("BlockAll");

    LINE(Start, End, FColor::Purple);  // debug trace

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, PROFILE_NAME,
                                             CoastalCharacterOwner->GetIgnoreCharacterParams()))
    {
        return std::make_optional<FVector>(HitResult.ImpactNormal);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FVector> UCoastalCharacterMovementComponent::GetHitNormalCharacterEquipment() const
{
    UCoastalEquipmentMeshComponent* Equipment = CoastalCharacterOwner->GetEquipmentMeshComponent();

    return Equipment->LineTraceCombined(CoastalCharacterOwner->GetIgnoreCharacterParams());
}

bool UCoastalCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UCoastalCharacterMovementComponent::SprintPressed()
{
    Safe_bWantsToSprint = true;
}

void UCoastalCharacterMovementComponent::SprintReleased()
{
    Safe_bWantsToSprint = false;
}

void UCoastalCharacterMovementComponent::SkatePressed()
{
    if (IsCustomMovementMode(CMOVE_Skate))
    {
        SetMovementMode(MOVE_Walking);
        DefaultLandMovementMode = MOVE_Walking;
    }
    else
    {
        SetMovementMode(MOVE_Custom, CMOVE_Skate);
        DefaultLandMovementMode = MOVE_Custom;
    }
}

#pragma endregion
