// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalCharacterMovementComponent.h"

#include "CoastalCharacter.h"
#include "CoastalEquipmentMeshComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

#if 1
constexpr float MacroDuration = 2.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !MacroDuration, MacroDuration);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !MacroDuration, MacroDuration);
#define CAPSULE(x, c)                                                                                                  \
    DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !MacroDuration, MacroDuration);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif

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

void UCoastalCharacterMovementComponent::PhysSkate(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }
    RestorePreAdditiveRootMotionVelocity();
}

bool UCoastalCharacterMovementComponent::GetHitResultCharacter(FHitResult& HitResult) const
{
    FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start
                  + CoastalCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f
                        * FVector::DownVector;
    static FName ProfileName = TEXT("BlockAll");

    LINE(Start, End, FColor::Purple);  // debug trace

    return GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, ProfileName,
                                                CoastalCharacterOwner->GetIgnoreCharacterParams());
}

bool UCoastalCharacterMovementComponent::GetHitResultCharacterEquipment(FVector& HitNormal) const
{
    UCoastalEquipmentMeshComponent* Equipment = CoastalCharacterOwner->GetEquipmentMeshComponent();

    // TODO: allow front and back surface contact bones to be set in `UCoastalEquipmentMeshComponent` blueprint

    FVector StartFront;
    FVector EndFront;
    FHitResult HitResultFront;
    bool bIsFrontOnSurface = GetWorld()->LineTraceSingleByChannel(HitResultFront, StartFront, EndFront, ECC_Visibility,
                                                                  CoastalCharacterOwner->GetIgnoreCharacterParams());

    FVector StartBack;
    FVector EndBack;
    FHitResult HitResultBack;
    bool bIsBackOnSurface = GetWorld()->LineTraceSingleByChannel(HitResultBack, StartBack, EndBack, ECC_Visibility,
                                                                 CoastalCharacterOwner->GetIgnoreCharacterParams());

    if (!bIsFrontOnSurface && !bIsBackOnSurface)
    {
        return false;
    }

    if (bIsFrontOnSurface && !bIsBackOnSurface)
    {
        HitNormal = HitResultFront.Normal;
    }
    else if (!bIsFrontOnSurface && bIsBackOnSurface)
    {
        HitNormal = HitResultBack.Normal;
    }
    else
    {
        // get average normal of the two hits
        HitNormal = (HitResultFront.Normal + HitResultBack.Normal).GetSafeNormal();
    }
    return true;
}