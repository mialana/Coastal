// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogCoastal, Log, All);

#if 1
constexpr float DEBUG_DURATION = 10.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, DEBUG_DURATION ? DEBUG_DURATION : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 15, c, !DEBUG_DURATION, DEBUG_DURATION);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !DEBUG_DURATION, DEBUG_DURATION);
#define CAPSULE(x, c)                                                                                                  \
    DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !DEBUG_DURATION, DEBUG_DURATION);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif
