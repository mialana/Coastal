// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "CoastalEquipmentMeshComponent.generated.h"

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

    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable) FVector GetFrontLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetFrontRightBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetBackLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable) FVector GetBackRightBoneLocation() const;

    std::optional<FHitResult> LineTraceFrontLeft(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceFrontRight(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceBackLeft(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceBackRight(const FCollisionQueryParams& IgnoreParams) const;

    std::optional<FVector> LineTraceCombined(const FCollisionQueryParams& IgnoreParams) const;
};
