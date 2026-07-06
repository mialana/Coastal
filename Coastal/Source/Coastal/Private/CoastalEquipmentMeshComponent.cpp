// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalEquipmentMeshComponent.h"

#include "Coastal.h"

DEFINE_LOG_CATEGORY(LogCoastalEquipment);

constexpr float kLINE_TRACE_DISTANCE = 5.f;

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
        UE_LOG(LogCoastalEquipment, Error, TEXT("Equipment front left bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(FrontLeftBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetFrontRightBoneLocation() const
{
    if (FrontRightBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastalEquipment, Error, TEXT("Equipment front right bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(FrontRightBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetBackLeftBoneLocation() const
{
    if (BackLeftBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastalEquipment, Error, TEXT("Equipment back left bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(BackLeftBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

FVector UCoastalEquipmentMeshComponent::GetBackRightBoneLocation() const
{
    if (BackRightBoneIndex == INDEX_NONE)
    {
        UE_LOG(LogCoastalEquipment, Error, TEXT("Equipment back right bone index was not set properly."));
    }

    FTransform Transform = GetBoneTransform(BackRightBoneIndex, FTransform::Identity);

    return Transform.GetLocation();
}

bool UCoastalEquipmentMeshComponent::LineTraceFrontLeft(FHitResult& OutHit,
                                                        const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontLeftBoneLocation();
    const FVector End = Start + kLINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, IgnoreParams);
}

bool UCoastalEquipmentMeshComponent::LineTraceFrontRight(FHitResult& OutHit,
                                                         const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetFrontRightBoneLocation();
    const FVector End = Start + kLINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, IgnoreParams);
}

bool UCoastalEquipmentMeshComponent::LineTraceBackLeft(FHitResult& OutHit,
                                                       const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackLeftBoneLocation();
    const FVector End = Start + kLINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, IgnoreParams);
}

bool UCoastalEquipmentMeshComponent::LineTraceBackRight(FHitResult& OutHit,
                                                        const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetBackRightBoneLocation();
    const FVector End = Start + kLINE_TRACE_DISTANCE * FVector::DownVector;

    LINE(Start, End, FColor::Purple);

    return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, IgnoreParams);
}
