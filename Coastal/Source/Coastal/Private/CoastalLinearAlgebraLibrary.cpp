// Copyright 2026, Amy Liu. All rights reserved.

#include "CoastalLinearAlgebraLibrary.h"

FVector UCoastalLinearAlgebraLibrary::RotateVectorTowards(const FVector& Current, const FVector& Target,
                                                                  float MaxDegreesThisTick)
{
    const float CosAngle = FMath::Clamp(FVector::DotProduct(Current, Target), -1.f, 1.f);
    const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(CosAngle));

    // within the allowed step, snap directly to target
    if (AngleDegrees <= MaxDegreesThisTick || AngleDegrees < KINDA_SMALL_NUMBER)
    {
        return Target;
    }

    FVector Axis = FVector::CrossProduct(Current, Target).GetSafeNormal();
    if (Axis.IsNearlyZero())
    {
        // angles are ~180 degrees apart. need to replace cross product with an arbitrary axis perpendicular to Current
        Axis = FVector::CrossProduct(Current, Current.Rotation().Quaternion().GetRightVector()).GetSafeNormal();
    }

    const FQuat DeltaRotation(Axis, FMath::DegreesToRadians(MaxDegreesThisTick));
    return DeltaRotation.RotateVector(Current).GetSafeNormal();
}