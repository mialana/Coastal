// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoastalLinearAlgebraLibrary.generated.h"

/**
 * 
 */
UCLASS()

class COASTAL_API UCoastalLinearAlgebraLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Coastal|Utilities|LinearAlgebra")
    static FVector RotateVectorTowards(const FVector& Current, const FVector& Target,
                                                                  float MaxDegreesThisTick);
};
