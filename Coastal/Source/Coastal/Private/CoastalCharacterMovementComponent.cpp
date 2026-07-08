// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalCharacterMovementComponent.h"

#include "Coastal.h"
#include "CoastalCharacter.h"
#include "CoastalEquipmentMeshComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

bool UCoastalCharacterMovementComponent::FSavedMove_Coastal::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                            ACharacter* InCharacter,
                                                                            float MaxDelta) const
{
    FSavedMove_Coastal* NewCoastalMove = static_cast<FSavedMove_Coastal*>(NewMove.Get());

    if (Saved_bWantsToSprint != NewCoastalMove->Saved_bWantsToSprint)
    {
        return false;
    }

    // defer to super class
    return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::Clear()
{
    FSavedMove_Character::Clear();

    // reset flags
    Saved_bWantsToSprint = 0u;
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

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::SetMoveFor(
    ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(
        C->GetCharacterMovement());

    Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void UCoastalCharacterMovementComponent::FSavedMove_Coastal::PrepMoveFor(ACharacter* C)
{
    FSavedMove_Character::PrepMoveFor(C);

    UCoastalCharacterMovementComponent* CharacterMovement = Cast<UCoastalCharacterMovementComponent>(
        C->GetCharacterMovement());

    CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

UCoastalCharacterMovementComponent::FNetworkPredictionData_Client_Coastal::FNetworkPredictionData_Client_Coastal(
    const UCharacterMovementComponent& ClientMovement)
    : FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr UCoastalCharacterMovementComponent::FNetworkPredictionData_Client_Coastal::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_Coastal());
}

UCoastalCharacterMovementComponent::UCoastalCharacterMovementComponent()
{
    NavAgentProps.bCanCrouch = true;
}

FNetworkPredictionData_Client* UCoastalCharacterMovementComponent::GetPredictionData_Client() const
{
    check(PawnOwner != nullptr);

    if (ClientPredictionData == nullptr)
    {
        UCoastalCharacterMovementComponent* MutableThis = const_cast<UCoastalCharacterMovementComponent*>(this);

        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Coastal(*this);

        // parameters for prediction
        MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
        MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
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

void UCoastalCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                           const FVector& OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

    if (MovementMode == MOVE_Walking)
    {
        if (Safe_bWantsToSprint)
        {
            MaxWalkSpeed = MaxSpeed_Sprint;
        }
        else
        {
            MaxWalkSpeed = MaxSpeed_Walk;
        }
    }
}

void UCoastalCharacterMovementComponent::SprintPressed()
{
    Safe_bWantsToSprint = true;
}

void UCoastalCharacterMovementComponent::SprintReleased()
{
    Safe_bWantsToSprint = false;
}

void UCoastalCharacterMovementComponent::CrouchPressed()
{
    bWantsToCrouch = !bWantsToCrouch;
}

void UCoastalCharacterMovementComponent::EnterSkate(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode) {}

void UCoastalCharacterMovementComponent::ExitSkate() {}

bool UCoastalCharacterMovementComponent::CanSkate() const
{
    return true;
}

void UCoastalCharacterMovementComponent::PhysSkate(float DeltaTime, int32 Iterations)
{
    // proceed only on tick
    if (DeltaTime < MIN_TICK_TIME)
    {
        return;
    }

    // do not double-count additive root motion
    RestorePreAdditiveRootMotionVelocity();

    std::optional<FVector> OptionHitNormal = GetHitNormalCharacterEquipment();

#if 0
    if (!OptionHitNormal.has_value() || Velocity.SizeSquared() < pow(MinSpeed_Skate, 2))
    {
        ExitSkate();
        StartNewPhysics(DeltaTime, Iterations);
        return;
    }
#endif

    FVector HitNormal = OptionHitNormal.value();

    // update velocity as a function of acceleration
    Velocity += GravityForce_Skate * FVector::DownVector * DeltaTime;

    // calculate effects of friction on velocity and acceleration
    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        CalcVelocity(DeltaTime, Friction_Skate, true, GetMaxBrakingDeceleration());
    }

    ApplyRootMotionToVelocity(DeltaTime);
    Iterations++;
    bJustTeleported = false;

    // compute new rotation given desired hit plane
    const FVector VelocityPlaneNormal = FVector::VectorPlaneProject(Velocity, HitNormal).GetSafeNormal();
    const FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelocityPlaneNormal, HitNormal).ToQuat();

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

std::optional<FVector> UCoastalCharacterMovementComponent::GetHitNormalCharacter() const
{
    FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start
                  + CoastalCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f
                        * FVector::DownVector;

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
