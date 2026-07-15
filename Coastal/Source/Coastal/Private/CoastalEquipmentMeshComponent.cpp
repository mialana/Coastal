// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalEquipmentMeshComponent.h"

#include "AnimationRuntime.h"
#include "AudioMixerChannel.h"
#include "Coastal.h"
#include "Engine/SkeletalMesh.h"

UCoastalEquipmentMeshComponent::UCoastalEquipmentMeshComponent() {}

void UCoastalEquipmentMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    ScaledHalfHeight = Bounds.BoxExtent.Z;
}

FName UCoastalEquipmentMeshComponent::GetAxisBoneName(ECoastalEquipmentAxisBone AxisBone) const
{
    switch (AxisBone)
    {
        case ECoastalEquipmentAxisBone::FrontLeft:
            return FrontLeftAxisBoneName;
        case ECoastalEquipmentAxisBone::FrontRight:
            return FrontRightAxisBoneName;
        case ECoastalEquipmentAxisBone::BackLeft:
            return BackLeftAxisBoneName;
        case ECoastalEquipmentAxisBone::BackRight:
            return BackRightAxisBoneName;
        default:
            checkNoEntry();
            return NAME_None;
    }
}

FVector UCoastalEquipmentMeshComponent::GetAxisBoneLocation(ECoastalEquipmentAxisBone AxisBone) const
{
    const FVector Location = GetBoneLocation(GetAxisBoneName(AxisBone), EBoneSpaces::WorldSpace);
    if (Location.IsZero())
    {
        UE_LOG(LogCoastal, Error, TEXT("Equipment axis bone name not set properly: '%s'"), *UEnum::GetValueAsString(AxisBone));
    }
    return Location;
}

bool UCoastalEquipmentMeshComponent::LineTraceAxisBone(ECoastalEquipmentAxisBone AxisBone, FHitResult& HitResult,
                                                       const FCollisionQueryParams& IgnoreParams) const
{
    const FVector Start = GetAxisBoneLocation(AxisBone);
    const FVector End = Start + ScaledHalfHeight * FVector::DownVector;

    bool bLineTrace = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, IgnoreParams);
    if (bLineTrace)
    {
        LINE(Start, End, FColor::White);
    }
    return bLineTrace;
}

bool UCoastalEquipmentMeshComponent::LineTraceCombined(FVector& AverageHitNormal, FHitResult& HitResultFrontLeft,
                                                       FHitResult& HitResultFrontRight, FHitResult& HitResultBackLeft,
                                                       FHitResult& HitResultBackRight,
                                                       const FCollisionQueryParams& IgnoreParams) const
{
    bool bDidHitFrontLeft = LineTraceAxisBone(ECoastalEquipmentAxisBone::FrontLeft, HitResultFrontLeft, IgnoreParams);
    bool bDidHitFrontRight = LineTraceAxisBone(ECoastalEquipmentAxisBone::FrontRight, HitResultFrontRight, IgnoreParams);
    bool bDidHitBackLeft = LineTraceAxisBone(ECoastalEquipmentAxisBone::BackLeft, HitResultBackLeft, IgnoreParams);
    bool bDidHitBackRight = LineTraceAxisBone(ECoastalEquipmentAxisBone::BackRight, HitResultBackRight, IgnoreParams);

    if (!bDidHitFrontLeft && !bDidHitFrontRight && !bDidHitBackLeft && !bDidHitBackRight)
    {
        return false;
    }

    FVector AccumulatedHitNormal = FVector::ZeroVector;
    int32 HitCount = 0;
    if (bDidHitFrontLeft)
    {
        AccumulatedHitNormal += HitResultFrontLeft.Normal;
        HitCount++;
    }
    if (bDidHitFrontRight)
    {
        AccumulatedHitNormal += HitResultFrontRight.Normal;
        HitCount++;
    }
    if (bDidHitBackLeft)
    {
        AccumulatedHitNormal += HitResultBackLeft.Normal;
        HitCount++;
    }
    if (bDidHitBackRight)
    {
        AccumulatedHitNormal += HitResultBackRight.Normal;
        HitCount++;
    }

    AverageHitNormal = (AccumulatedHitNormal / HitCount).GetSafeNormal();
    return true;
}

bool UCoastalEquipmentMeshComponent::LineTraceCombined(FVector& AverageHitNormal,
                                                       const FCollisionQueryParams& IgnoreParams) const
{
    FHitResult HitResultFrontLeft;
    FHitResult HitResultFrontRight;
    FHitResult HitResultBackLeft;
    FHitResult HitResultBackRight;
    return LineTraceCombined(AverageHitNormal, HitResultFrontLeft, HitResultFrontRight, HitResultBackLeft, HitResultBackRight,
                             IgnoreParams);
}
