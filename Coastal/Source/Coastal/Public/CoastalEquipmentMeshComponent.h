// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "CoastalEquipmentMeshComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoastalEquipment, Log, All);

/**
 * 
 */
UCLASS()

class COASTAL_API UCoastalEquipmentMeshComponent : public USkeletalMeshComponent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FName FrontLeftBoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FName FrontRightBoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FName BackLeftBoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    FName BackRightBoneName;

protected:
    UPROPERTY(BlueprintReadOnly) int32 FrontLeftBoneIndex = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly) int32 FrontRightBoneIndex = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly) int32 BackLeftBoneIndex = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly) int32 BackRightBoneIndex = INDEX_NONE;

public:
    UCoastalEquipmentMeshComponent();

public:
    UFUNCTION(BlueprintCallable) FVector GetFrontLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetFrontRightBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetBackLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetBackRightBoneLocation() const;

    bool LineTraceFrontLeft(FHitResult& OutHit, const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceFrontRight(FHitResult& OutHit, const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceBackLeft(FHitResult& OutHit, const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceBackRight(FHitResult& OutHit, const FCollisionQueryParams& IgnoreParams) const;
};
