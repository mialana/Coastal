// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogCoastal, Log, All);

#if 1
constexpr float kDEBUG_DURATION = 2.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, kDEBUG_DURATION ? kDEBUG_DURATION : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !kDEBUG_DURATION, kDEBUG_DURATION);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !kDEBUG_DURATION, kDEBUG_DURATION);
#define CAPSULE(x, c)                                                                                                  \
    DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !kDEBUG_DURATION, kDEBUG_DURATION);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif
