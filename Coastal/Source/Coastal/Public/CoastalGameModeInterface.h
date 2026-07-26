// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoastalGameModeInterface.generated.h"

UENUM(BlueprintType)

enum EMovementSurfaceType
{
    MovementSurface_Slow UMETA(DisplayName = "Slow Movement"),
    MovementSurface_Default UMETA(DisplayName = "Default Movement"),
    MovementSurface_Fast UMETA(DisplayName = "Fast Movement"),
    MovementSurface_MAX UMETA(Hidden),
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)

class UCoastalGameModeInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class COASTAL_API ICoastalGameModeInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameMode|Coastal")
    void OnGenerateNextGround();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "GameMode|Coastal")
    EMovementSurfaceType SurfaceTypeToMovementSurfaceType(const TEnumAsByte<EPhysicalSurface>& InSurfaceType);
};
