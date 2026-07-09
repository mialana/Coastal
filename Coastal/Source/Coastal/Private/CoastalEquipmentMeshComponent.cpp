// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalEquipmentMeshComponent.h"

#include "Coastal.h"

constexpr float LINE_TRACE_DISTANCE = 5.f;

UCoastalEquipmentMeshComponent::UCoastalEquipmentMeshComponent()
{
    FrontLeftBoneIndex = GetBoneIndex(FrontLeftBoneName);
    FrontRightBoneIndex = GetBoneIndex(FrontRightBoneName);
    BackLeftBoneIndex = GetBoneIndex(BackLeftBoneName);
    BackRightBoneIndex = GetBoneIndex(BackRightBoneName);
}

FVector UCoastalEquipmentMeshComponent::GetFrontLeftBoneLocation() const
{
    if (FrontLeftBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment front left bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(FrontLeftBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetFrontRightBoneLocation() const
{
    if (FrontRightBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment front right bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(FrontRightBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetBackLeftBoneLocation() const
{
    if (BackLeftBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment back left bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(BackLeftBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetBackRightBoneLocation() const
{
    if (BackRightBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment back right bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(BackRightBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceFrontLeft(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontLeftBoneLocation();
    const FVector End = Start + LINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceFrontRight(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontRightBoneLocation();
    const FVector End = Start + LINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceBackLeft(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackLeftBoneLocation();
    const FVector End = Start + LINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
        return std::make_optional<FHitResult>(HitResult);
    }
    return std::nullopt;  // no hit occurred
}

std::optional<FHitResult> UCoastalEquipmentMeshComponent::LineTraceBackRight(
    const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackRightBoneLocation();
    const FVector End = Start + LINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams))
    {
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

    return std::nullopt;
}
