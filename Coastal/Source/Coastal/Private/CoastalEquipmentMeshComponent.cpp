// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalEquipmentMeshComponent.h"

#include "AnimationRuntime.h"
#include "Coastal.h"
#include "Engine/SkeletalMesh.h"

UCoastalEquipmentMeshComponent::UCoastalEquipmentMeshComponent() {}

void UCoastalEquipmentMeshComponent::InitializeComponent()
{
    Super::InitializeComponent();

    SetVisibility(false);
}

void UCoastalEquipmentMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    ScaledHalfHeight = Bounds.BoxExtent.Z;
}

FVector UCoastalEquipmentMeshComponent::GetFrontLeftBoneLocation() const
{
    FVector Location = GetBoneLocation(FrontLeftBoneName, EBoneSpaces::WorldSpace);
    if (Location.IsZero())
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment front left bone name was not set properly."));
    }

    return Location;
}

FVector UCoastalEquipmentMeshComponent::GetFrontRightBoneLocation() const
{
    FVector Location = GetBoneLocation(FrontRightBoneName, EBoneSpaces::WorldSpace);
    if (Location.IsZero())
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment front right bone name was not set properly."));
    }

    return Location;
}

FVector UCoastalEquipmentMeshComponent::GetBackLeftBoneLocation() const
{
    FVector Location = GetBoneLocation(BackLeftBoneName, EBoneSpaces::WorldSpace);
    if (Location.IsZero())
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment back left bone name was not set properly."));
    }

    return Location;
}

FVector UCoastalEquipmentMeshComponent::GetBackRightBoneLocation() const
{
    FVector Location = GetBoneLocation(BackRightBoneName, EBoneSpaces::WorldSpace);
    if (Location.IsZero())
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment back right bone name was not set properly."));
    }

    return Location;
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceFrontLeft(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontLeftBoneLocation();
    const FVector End = Start + ScaledHalfHeight * FVector::DownVector;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        LINE(Start, End, FColor::Orange);
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceFrontRight(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontRightBoneLocation();
    const FVector End = Start + ScaledHalfHeight * FVector::DownVector;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        LINE(Start, End, FColor::Purple);
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceBackLeft(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackLeftBoneLocation();
    const FVector End = Start + ScaledHalfHeight * FVector::DownVector;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        LINE(Start, End, FColor::Red);
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceBackRight(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackRightBoneLocation();
    const FVector End = Start + ScaledHalfHeight * FVector::DownVector;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        LINE(Start, End, FColor::Blue);
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FVector> UCoastalEquipmentMeshComponent::LineTraceCombined(const FCollisionQueryParams& IgnoreParams) const
{
    std::optional<FHitResult> OptionHitResultFrontLeft = LineTraceFrontLeft(IgnoreParams);
    bool bDidHitFrontLeft = OptionHitResultFrontLeft.has_value();

    std::optional<FHitResult> OptionHitResultFrontRight = LineTraceFrontRight(IgnoreParams);
    bool bDidHitFrontRight = OptionHitResultFrontRight.has_value();

    std::optional<FHitResult> OptionHitResultBackLeft = LineTraceBackLeft(IgnoreParams);
    bool bDidHitBackLeft = OptionHitResultBackLeft.has_value();

    std::optional<FHitResult> OptionHitResultBackRight = LineTraceBackRight(IgnoreParams);
    bool bDidHitBackRight = OptionHitResultBackRight.has_value();

    if (!bDidHitFrontLeft && !bDidHitFrontRight && !bDidHitBackLeft && !bDidHitBackRight)
    {
        return std::nullopt;
    }

    FVector AccumulatedNormal = FVector::ZeroVector;
    int32 HitCount = 0;
    if (bDidHitFrontLeft)
    {
        AccumulatedNormal += OptionHitResultFrontLeft->Normal;
        HitCount++;
    }
    if (bDidHitFrontRight)
    {
        AccumulatedNormal += OptionHitResultFrontRight->Normal;
        HitCount++;
    }
    if (bDidHitBackLeft)
    {
        AccumulatedNormal += OptionHitResultBackLeft->Normal;
        HitCount++;
    }
    if (bDidHitBackRight)
    {
        AccumulatedNormal += OptionHitResultBackRight->Normal;
        HitCount++;
    }

    return (AccumulatedNormal / HitCount).GetSafeNormal();
}
