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

public:
    UCoastalEquipmentMeshComponent();

    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal") FVector GetFrontLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal") FVector GetFrontRightBoneLocation() const;
    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal") FVector GetBackLeftBoneLocation() const;
    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal") FVector GetBackRightBoneLocation() const;

    std::optional<FHitResult> LineTraceFrontLeft(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceFrontRight(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceBackLeft(const FCollisionQueryParams& IgnoreParams) const;
    std::optional<FHitResult> LineTraceBackRight(const FCollisionQueryParams& IgnoreParams) const;

    std::optional<FVector> LineTraceCombined(const FCollisionQueryParams& IgnoreParams) const;

public:
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName FrontLeftBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName FrontRightBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName BackLeftBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName BackRightBoneName;

    UPROPERTY(Transient) float ScaledHalfHeight;
};
