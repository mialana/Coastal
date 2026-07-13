// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalCharacterMovementComponent.h"

#include "Coastal.h"
#include "CoastalCharacter.h"
#include "CoastalEquipmentMeshComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

#pragma region FSavedMove

void FSavedMove_Coastal::Clear()
{
    FSavedMove_Character::Clear();

    // reset flags
    Saved_bWantsToSprint = 0u;
}

void FSavedMove_Coastal::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                    FNetworkPredictionData_Client_Character& ClientData)
{
    FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(C->GetCharacterMovement());

    Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

bool FSavedMove_Coastal::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
    FSavedMove_Coastal* NewCoastalMove = static_cast<FSavedMove_Coastal*>(NewMove.Get());

    if (Saved_bWantsToSprint != NewCoastalMove->Saved_bWantsToSprint)
    {
        return false;
    }

    // defer to super class
    return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_Coastal::PrepMoveFor(ACharacter* C)
{
    FSavedMove_Character::PrepMoveFor(C);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(C->GetCharacterMovement());

    CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

uint8 FSavedMove_Coastal::GetCompressedFlags() const
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

FNetworkPredictionData_Client_Coastal::FNetworkPredictionData_Client_Coastal(const UCharacterMovementComponent& ClientMovement)
    : FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_Coastal::AllocateNewMove()
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
    CoastalCharacterOwner->GetEquipmentMeshComponent()->SetVisibility(false);
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

    bJustTeleported = false;

    std::optional<FVector> OptionHitNormal = GetHitNormalCharacterEquipment();
    FVector HitNormal = OptionHitNormal.has_value() ? OptionHitNormal.value() : FVector::UpVector;

    // update velocity as a function of acceleration
    Velocity += GetGravityZ() * FVector::UpVector * DeltaTime;

    // calculate effects of friction on velocity and acceleration
    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        CalcVelocity(DeltaTime, FrictionFactorSkating, false, GetMaxBrakingDeceleration());
    }

    ApplyRootMotionToVelocity(DeltaTime);
    Iterations++;

    // flatten forward vector onto hit surface by removing component that points along normal
    FVector Forward = Velocity.IsNearlyZero() ? GetForwardVector() : Velocity;
    FVector ProjectedForward = FVector::VectorPlaneProject(Forward, HitNormal).GetSafeNormal();

    // compute rotation from desired forward (projected velocity) and desired up
    const FQuat NewRotation = FRotationMatrix::MakeFromXZ(ProjectedForward, HitNormal).ToQuat();

    // compute displacement during this tick
    const FVector Displacement = Velocity * DeltaTime;

    // save location before movement
    const FVector OldLocation = UpdatedComponent->GetComponentLocation();

    // perform the actual movement
    FHitResult SafeMoveHitResult(1.f);
    SafeMoveUpdatedComponent(Displacement, NewRotation, true, SafeMoveHitResult);

    // check for if anything was hit during movement
    if (SafeMoveHitResult.Time < 1.f)
    {
        HandleImpact(SafeMoveHitResult, DeltaTime, Displacement);
        SlideAlongSurface(Displacement, 1.f - SafeMoveHitResult.Time, SafeMoveHitResult.Normal, SafeMoveHitResult, true);
    }

    // adjust velocity & acceleration in case of impact during safe movement
    if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
    }
}

// Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt

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
