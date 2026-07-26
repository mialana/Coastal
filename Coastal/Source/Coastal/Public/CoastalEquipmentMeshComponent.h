// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "CoastalEquipmentMeshComponent.generated.h"

UENUM(BlueprintType)
enum class ECoastalEquipmentAxisBone : uint8
{
    FrontLeft,
    FrontRight,
    BackLeft,
    BackRight
};

UCLASS()

class COASTAL_API UCoastalEquipmentMeshComponent : public USkeletalMeshComponent
{
    GENERATED_BODY()

public:
    UCoastalEquipmentMeshComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal")
    FName GetAxisBoneName(ECoastalEquipmentAxisBone AxisBone) const;
    UFUNCTION(BlueprintCallable, Category = "Components|SkeletalMesh|Coastal")
    FVector GetAxisBoneLocation(ECoastalEquipmentAxisBone AxisBone) const;

    bool LineTraceAxisBone(ECoastalEquipmentAxisBone AxisBone, FHitResult& HitResult,
                           const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceCombined(FVector& AverageHitNormal, FHitResult& HitResultFrontLeft, FHitResult& HitResultFrontRight,
                           FHitResult& HitResultBackLeft, FHitResult& HitResultBackRight,
                           const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceCombined(FVector& AverageHitNormal, const FCollisionQueryParams& IgnoreParams) const;
    bool LineTraceRootComponent(FHitResult& HitResult, const FCollisionQueryParams& QueryParams) const;

public:
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName FrontLeftAxisBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName FrontRightAxisBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName BackLeftAxisBoneName;
    UPROPERTY(Category = "CoastalEquipmentMeshComponent", EditAnywhere, BlueprintReadWrite) FName BackRightAxisBoneName;

    UPROPERTY(Transient) float TraceLength;
};
